// oxlint-disable func-style, max-lines, no-use-before-define
import { existsSync } from 'node:fs';
import { resolve } from 'node:path';
import * as three from 'three';
import { Screen, init } from '@node-3d/core';
import { friends, steam, update as updateSteam, user, userStats, utils } from '@node-3d/steam-api';

type TKeyboardEvent = Readonly<{
	code?: string;
	key?: string;
}>;

type TBody = Readonly<{
	mesh: three.Object3D;
	position: three.Vector2;
	velocity: three.Vector2;
	radius: number;
}>;

type TShip = TBody &
	Readonly<{
		flame: three.Object3D;
		nose: three.Object3D;
	}>;

type TShot = TBody & {
	age: number;
};

type TAsteroid = TBody & {
	spin: number;
};

const SpacewarAppId = 480;
const ArenaWidth = 760;
const ArenaHeight = 520;
const ShipRadius = 18;
const ShipTurnSpeed = 4.8;
const ShipThrust = 360;
const ShipDrag = 0.985;
const ShotSpeed = 470;
const ShotLifetime = 1.25;
const ShotCooldown = 0.15;
const MaxHeat = 1;
const HeatPerShot = 0.18;
const HeatDissipationPerSecond = 0.34;
const AsteroidCount = 10;
const ShieldDrainPerSecond = 0.7;
const ShieldRecoverPerSecond = 0.28;

const appIdPath = resolve(process.cwd(), 'steam_appid.txt');
if (!existsSync(appIdPath)) {
	throw new Error('Run this example from examples/spacewar so Steam can find steam_appid.txt.');
}

const steamResult = steam.initEx();
if (!steamResult.ok) {
	throw new Error(
		`SteamAPI_InitEx failed with ${steamResult.result}: ${steamResult.errorMessage}`,
	);
}

const appId = utils.getAppId();
if (appId !== SpacewarAppId) {
	console.warn(`Expected Spacewar AppID ${SpacewarAppId}, got ${appId}.`);
}

const { loop } = init({
	isGles3: true,
	isWebGL2: true,
	autoEsc: true,
	autoFullscreen: true,
	vsync: true,
	title: 'Node3D Spacewar',
});

const screen = new Screen({ three, far: 2000 });
screen.camera.position.z = 620;
screen.renderer.setClearColor(0x050713, 1);

const pressedKeys = new Set<string>();
const shots: TShot[] = [];
const asteroids: TAsteroid[] = [];

let shipAngle = Math.PI / 2;
let score = 0;
let lives = 3;
let heat = 0;
let shield = 1;
let invincibleFor = 1.5;
let timeSinceShot = 0;
let previousNow = 0;
let lastPresenceAt = 0;
let gameOver = false;
let gameOverLogged = false;

const starField = new three.Group();
const asteroidGroup = new three.Group();
const shotGroup = new three.Group();
const hudGroup = new three.Group();
const gameOverGroup = createGameOverMarker();

screen.scene.add(starField, asteroidGroup, shotGroup, hudGroup, gameOverGroup);

const ship = createShip();
screen.scene.add(ship.mesh);

const shieldMesh = new three.Mesh(
	new three.RingGeometry(ShipRadius + 5, ShipRadius + 7, 40),
	new three.MeshBasicMaterial({
		color: 0x65d8ff,
		transparent: true,
		opacity: 0.25,
		side: three.DoubleSide,
	}),
);
screen.scene.add(shieldMesh);

const heatBar = createBar(-ArenaWidth / 2, -ArenaHeight / 2 - 28, 180, 10, 0xff6b4a);
const shieldBar = createBar(-ArenaWidth / 2, -ArenaHeight / 2 - 45, 180, 10, 0x48c7ff);
const lifeBar = createBar(ArenaWidth / 2 - 120, -ArenaHeight / 2 - 28, 120, 10, 0xa8ff78);
hudGroup.add(heatBar.root, shieldBar.root, lifeBar.root);

console.log('Node3D Spacewar');
console.log('Controls: W thrust, A/D turn, Space fire, Shift shield, R restart.');
console.log('Steam: F1 Friends, F2 Achievements, F3 Community, F4 Store, F5 Workshop.');
console.log('Steam account:', user.getSteamId(), user.isLoggedOn() ? 'logged on' : 'offline');
console.log('Achievements:', userStats.getAchievementNames());

friends.setRichPresence('status', 'Playing Node3D Spacewar');
friends.setRichPresence('steam_display', '#Status_Generic');
friends.setRichPresence('score', '0');

process.once('exit', () => {
	friends.clearRichPresence();
});

screen.on('keydown', (event) => {
	const code = getEventCode(event as TKeyboardEvent);
	if (!code) {
		return;
	}

	if (!pressedKeys.has(code)) {
		handleKeyPress(code);
	}
	pressedKeys.add(code);
});

screen.on('keyup', (event) => {
	const code = getEventCode(event as TKeyboardEvent);
	if (code) {
		pressedKeys.delete(code);
	}
});

seedStars();
resetGame();

loop((now) => {
	const dt = Math.min((now - previousNow) / 1000 || 1 / 60, 0.05);
	previousNow = now;

	pumpSteamCallbacks();

	if (!gameOver) {
		updateShip(dt);
		resolveCollisions();
	} else if (isPressed('KeyR')) {
		resetGame();
	}

	updateShots(dt);
	updateAsteroids(dt);
	updateGameOverMarker(now);
	updateHud();
	updatePresence(now);
	screen.draw();
});

function createShip(): TShip {
	const group = new three.Group();
	const outlineGeometry = new three.BufferGeometry();
	outlineGeometry.setAttribute(
		'position',
		new three.BufferAttribute(
			new Float32Array([30, 0, -14, 15, -7, 5, -18, 0, -7, -5, -14, -15, 30, 0]),
			3,
		),
	);

	const outline = new three.Line(
		outlineGeometry,
		new three.LineBasicMaterial({
			color: 0xdaf7ff,
		}),
	);
	const nose = new three.Mesh(
		new three.ConeGeometry(7, 16, 3),
		new three.MeshBasicMaterial({ color: 0xffe16b }),
	);
	const cockpit = new three.Mesh(
		new three.CircleGeometry(4, 16),
		new three.MeshBasicMaterial({ color: 0x80e8ff }),
	);
	const flame = new three.Mesh(
		new three.ConeGeometry(7, 22, 3),
		new three.MeshBasicMaterial({
			color: 0xff7a2f,
			transparent: true,
			opacity: 0.85,
		}),
	);

	nose.position.set(29, 0, 1);
	nose.rotation.z = -Math.PI / 2;
	cockpit.position.set(3, 0, 1);
	flame.position.set(-24, 0, 1);
	flame.rotation.z = Math.PI / 2;
	flame.visible = false;
	group.add(outline, nose, cockpit, flame);

	return {
		mesh: group,
		flame,
		nose,
		position: new three.Vector2(),
		velocity: new three.Vector2(),
		radius: ShipRadius,
	};
}

function createAsteroid(radius: number): TAsteroid {
	const points: three.Vector2[] = [];
	for (let index = 0; index < 14; index++) {
		const angle = (index / 14) * Math.PI * 2;
		const jitter = 0.72 + Math.random() * 0.42;
		points.push(
			new three.Vector2(Math.cos(angle) * radius * jitter, Math.sin(angle) * radius * jitter),
		);
	}
	points.push(points[0].clone());

	const mesh = new three.Line(
		new three.BufferGeometry().setFromPoints(points),
		new three.LineBasicMaterial({ color: 0x9ca8ff }),
	);
	asteroidGroup.add(mesh);

	return {
		mesh,
		position: randomEdgePosition(),
		velocity: randomVelocity(35, 105),
		radius,
		spin: (Math.random() - 0.5) * 1.8,
	};
}

function createShot(): void {
	if (timeSinceShot < ShotCooldown || heat >= MaxHeat) {
		return;
	}

	const direction = new three.Vector2(Math.cos(shipAngle), Math.sin(shipAngle));
	const mesh = new three.Mesh(
		new three.CircleGeometry(3, 12),
		new three.MeshBasicMaterial({ color: 0xffef89 }),
	);
	const shot: TShot = {
		mesh,
		position: ship.position.clone().addScaledVector(direction, ShipRadius + 8),
		velocity: ship.velocity.clone().addScaledVector(direction, ShotSpeed),
		radius: 3,
		age: 0,
	};

	shots.push(shot);
	shotGroup.add(mesh);
	timeSinceShot = 0;
	heat = Math.min(MaxHeat, heat + HeatPerShot);
}

function seedStars(): void {
	const geometry = new three.BufferGeometry();
	const vertices: number[] = [];
	for (let index = 0; index < 260; index++) {
		vertices.push(
			Math.random() * (ArenaWidth + 120) - ArenaWidth / 2 - 60,
			Math.random() * (ArenaHeight + 120) - ArenaHeight / 2 - 60,
			-60 - Math.random() * 40,
		);
	}
	geometry.setAttribute('position', new three.Float32BufferAttribute(vertices, 3));
	starField.add(
		new three.Points(
			geometry,
			new three.PointsMaterial({
				color: 0x7f96c7,
				size: 1.5,
			}),
		),
	);
}

function resetGame(): void {
	score = 0;
	lives = 3;
	heat = 0;
	shield = 1;
	invincibleFor = 1.5;
	gameOver = false;
	gameOverLogged = false;
	timeSinceShot = ShotCooldown;
	ship.position.set(0, 0);
	ship.velocity.set(0, 0);
	shipAngle = Math.PI / 2;
	ship.mesh.visible = true;
	gameOverGroup.visible = false;
	clearShots();
	clearAsteroids();
	for (let index = 0; index < AsteroidCount; index++) {
		asteroids.push(createAsteroid(18 + Math.random() * 24));
	}
}

function updateShip(dt: number): void {
	timeSinceShot += dt;
	heat = Math.max(0, heat - HeatDissipationPerSecond * dt);
	invincibleFor = Math.max(0, invincibleFor - dt);

	if (isPressed('KeyA') || isPressed('ArrowLeft')) {
		shipAngle += ShipTurnSpeed * dt;
	}
	if (isPressed('KeyD') || isPressed('ArrowRight')) {
		shipAngle -= ShipTurnSpeed * dt;
	}
	if (isPressed('KeyW') || isPressed('ArrowUp')) {
		ship.velocity.addScaledVector(
			new three.Vector2(Math.cos(shipAngle), Math.sin(shipAngle)),
			ShipThrust * dt,
		);
	}
	if (isPressed('Space')) {
		createShot();
	}

	const wantsShield = isPressed('ShiftLeft') || isPressed('ShiftRight');
	shield =
		wantsShield && shield > 0
			? Math.max(0, shield - ShieldDrainPerSecond * dt)
			: Math.min(1, shield + ShieldRecoverPerSecond * dt);

	ship.velocity.multiplyScalar(ShipDrag);
	ship.position.addScaledVector(ship.velocity, dt);
	wrap(ship);
	ship.mesh.position.set(ship.position.x, ship.position.y, 0);
	ship.mesh.rotation.z = shipAngle;
	ship.flame.visible = isPressed('KeyW') || isPressed('ArrowUp');
	ship.flame.scale.x = 0.8 + Math.random() * 0.35;

	const shieldActive = wantsShield && shield > 0;
	shieldMesh.visible = shieldActive || invincibleFor > 0;
	shieldMesh.position.copy(ship.mesh.position);
	const shieldMaterial = shieldMesh.material as three.MeshBasicMaterial;
	shieldMaterial.opacity = shieldActive
		? 0.35
		: 0.14 + Math.sin(performance.now() * 0.018) * 0.08;
}

function updateShots(dt: number): void {
	for (let index = shots.length - 1; index >= 0; index--) {
		const shot = shots[index];
		shot.age += dt;
		shot.position.addScaledVector(shot.velocity, dt);
		wrap(shot);
		shot.mesh.position.set(shot.position.x, shot.position.y, 0);

		if (shot.age > ShotLifetime) {
			removeShot(index);
		}
	}
}

function updateAsteroids(dt: number): void {
	for (const asteroid of asteroids) {
		asteroid.position.addScaledVector(asteroid.velocity, dt);
		wrap(asteroid);
		asteroid.mesh.position.set(asteroid.position.x, asteroid.position.y, 0);
		asteroid.mesh.rotation.z += asteroid.spin * dt;
	}
}

function resolveCollisions(): void {
	for (let asteroidIndex = asteroids.length - 1; asteroidIndex >= 0; asteroidIndex--) {
		const asteroid = asteroids[asteroidIndex];
		for (let shotIndex = shots.length - 1; shotIndex >= 0; shotIndex--) {
			const shot = shots[shotIndex];
			if (shot.position.distanceTo(asteroid.position) > shot.radius + asteroid.radius) {
				continue;
			}

			score += Math.round(100 - asteroid.radius);
			removeShot(shotIndex);
			removeAsteroid(asteroidIndex);
			asteroids.push(createAsteroid(18 + Math.random() * 24));
			return;
		}

		const shieldActive = shieldMesh.visible && shield > 0;
		const collisionRadius = asteroid.radius + ShipRadius + (shieldActive ? 10 : 0);
		if (invincibleFor > 0 || asteroid.position.distanceTo(ship.position) > collisionRadius) {
			continue;
		}

		if (shieldActive) {
			score += 25;
			shield = Math.max(0, shield - 0.2);
			const normal = asteroid.position.clone().sub(ship.position).normalize();
			asteroid.velocity.addScaledVector(normal, -2 * asteroid.velocity.dot(normal));
			continue;
		}

		lives--;
		invincibleFor = 1.5;
		ship.position.set(0, 0);
		ship.velocity.set(0, 0);
		if (lives <= 0) {
			gameOver = true;
			ship.mesh.visible = false;
			gameOverGroup.visible = true;
			friends.setRichPresence('status', `Game over with ${score} points`);
			if (!gameOverLogged) {
				console.log(`Game over. Final score: ${score}. Press R to restart.`);
				gameOverLogged = true;
			}
		}
	}
}

function createGameOverMarker(): three.Group {
	const group = new three.Group();
	const material = new three.LineBasicMaterial({
		color: 0xff3f5f,
		transparent: true,
		opacity: 0.9,
	});
	const crossA = new three.Line(
		new three.BufferGeometry().setFromPoints([
			new three.Vector2(-70, -70),
			new three.Vector2(70, 70),
		]),
		material,
	);
	const crossB = new three.Line(
		new three.BufferGeometry().setFromPoints([
			new three.Vector2(-70, 70),
			new three.Vector2(70, -70),
		]),
		material,
	);
	const ring = new three.Mesh(
		new three.RingGeometry(88, 92, 64),
		new three.MeshBasicMaterial({
			color: 0xff3f5f,
			transparent: true,
			opacity: 0.28,
			side: three.DoubleSide,
		}),
	);

	group.add(crossA, crossB, ring);
	group.visible = false;
	return group;
}

function updateGameOverMarker(now: number): void {
	if (!gameOver) {
		return;
	}

	const pulse = 1 + Math.sin(now * 0.006) * 0.08;
	gameOverGroup.rotation.z += 0.012;
	gameOverGroup.scale.set(pulse, pulse, 1);
}

function updateHud(): void {
	heatBar.fill.scale.x = heat;
	shieldBar.fill.scale.x = shield;
	lifeBar.fill.scale.x = lives / 3;
	const heatMaterial = heatBar.fill.material as three.MeshBasicMaterial;
	heatMaterial.color.setHex(heat > 0.85 ? 0xff2d2d : 0xff6b4a);
}

function updatePresence(now: number): void {
	if (now - lastPresenceAt < 5000) {
		return;
	}

	lastPresenceAt = now;
	friends.setRichPresence('score', String(score));
	friends.setRichPresence(
		'status',
		gameOver ? `Game over: ${score}` : `Score ${score}, ${lives} lives`,
	);
}

function pumpSteamCallbacks(): void {
	for (const event of updateSteam()) {
		if (event.type === 'game-overlay-activated') {
			console.log(event.active ? 'Steam overlay opened.' : 'Steam overlay closed.');
		} else {
			console.log('Steam event:', event);
		}
	}
}

function handleKeyPress(code: string): void {
	if (code === 'KeyR') {
		resetGame();
		return;
	}

	if (code === 'F1') {
		friends.activateGameOverlay('Friends');
	} else if (code === 'F2') {
		friends.activateGameOverlay('Achievements');
	} else if (code === 'F3') {
		friends.activateGameOverlay('Community');
	} else if (code === 'F4') {
		friends.activateGameOverlayToStore(SpacewarAppId, 0);
	} else if (code === 'F5') {
		friends.activateGameOverlayToWebPage(`https://steamcommunity.com/app/${SpacewarAppId}`);
	}
}

function createBar(x: number, y: number, width: number, height: number, color: number) {
	const root = new three.Group();
	const background = new three.Mesh(
		new three.PlaneGeometry(width, height),
		new three.MeshBasicMaterial({
			color: 0x1b2540,
			transparent: true,
			opacity: 0.8,
		}),
	);
	const fill = new three.Mesh(
		new three.PlaneGeometry(width, height),
		new three.MeshBasicMaterial({ color }),
	);

	background.position.set(x + width / 2, y, 0);
	fill.position.set(x + width / 2, y, 1);
	fill.scale.x = 1;
	root.add(background, fill);
	return { root, fill };
}

function clearShots(): void {
	while (shots.length > 0) {
		removeShot(shots.length - 1);
	}
}

function clearAsteroids(): void {
	while (asteroids.length > 0) {
		removeAsteroid(asteroids.length - 1);
	}
}

function removeShot(index: number): void {
	const [shot] = shots.splice(index, 1);
	shotGroup.remove(shot.mesh);
}

function removeAsteroid(index: number): void {
	const [asteroid] = asteroids.splice(index, 1);
	asteroidGroup.remove(asteroid.mesh);
}

function wrap(body: TBody): void {
	const halfWidth = ArenaWidth / 2;
	const halfHeight = ArenaHeight / 2;
	if (body.position.x < -halfWidth) {
		body.position.x = halfWidth;
	} else if (body.position.x > halfWidth) {
		body.position.x = -halfWidth;
	}

	if (body.position.y < -halfHeight) {
		body.position.y = halfHeight;
	} else if (body.position.y > halfHeight) {
		body.position.y = -halfHeight;
	}
}

function randomEdgePosition(): three.Vector2 {
	const horizontal = Math.random() > 0.5;
	if (horizontal) {
		return new three.Vector2(
			Math.random() * ArenaWidth - ArenaWidth / 2,
			Math.random() > 0.5 ? -ArenaHeight / 2 : ArenaHeight / 2,
		);
	}

	return new three.Vector2(
		Math.random() > 0.5 ? -ArenaWidth / 2 : ArenaWidth / 2,
		Math.random() * ArenaHeight - ArenaHeight / 2,
	);
}

function randomVelocity(min: number, max: number): three.Vector2 {
	const angle = Math.random() * Math.PI * 2;
	const speed = min + Math.random() * (max - min);
	return new three.Vector2(Math.cos(angle), Math.sin(angle)).multiplyScalar(speed);
}

function isPressed(code: string): boolean {
	return pressedKeys.has(code);
}

function getEventCode(event: TKeyboardEvent): string | null {
	if (typeof event.code === 'string') {
		return event.code;
	}
	if (typeof event.key === 'string' && event.key.length === 1) {
		return `Key${event.key.toUpperCase()}`;
	}
	return typeof event.key === 'string' ? event.key : null;
}
