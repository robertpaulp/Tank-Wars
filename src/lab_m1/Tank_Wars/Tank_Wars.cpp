#include <vector>
#include <iostream>

#include "lab_m1/Tank_Wars/Tank_Wars.h"
#include "lab_m1/Tank_Wars/terrain.h"
#include "lab_m1/Tank_Wars/T1object2D.h"
#include "lab_m1/Tank_Wars/T1transform2D.h"


using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tank_Wars::Tank_Wars()
{
}


Tank_Wars::~Tank_Wars()
{
}

void Tank_Wars::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 corner = glm::vec3(0, 0, 0);

	// Var declaration
	terrain_points = terrain::generate_terrain(resolution.x, resolution.y);
	interval = resolution.x / static_cast<float>(terrain_points.size());
	tank1TurretOffset = make_tuple(0, 0);
	tank2TurretOffset = make_tuple(0, 0);
	collisionTol = 5.0f;
	radiusDeformation = 50.0f;
	slideTol = 7.0f;
	prevDeltaTimeSeconds = 0.0f;

	// -------------- Tank --------------
	tankSpeed = 7.5f;
	tankBarrelRotationSpeed = 0.05f;

	// -------------- Tank1 Vars --------------
	// --- Position ---
	tank1PosX = 100;
	tank1PosY = 100;
	tank1Angle = 0;
	tank1BarrelAngle = M_PI / 4;
	// --- Health ---
	tank1Health = 100;


	// -------------- Tank2 Vars --------------
	// --- Position ---
	tank2PosX = 1000;
	tank2PosY = 100;
	tank2Angle = 0;
	tank2BarrelAngle = 3 * M_PI / 4;
	// --- Health ---
	tank2Health = 100;

	// -------------- Projectile Vars --------------
	projectileDamage = 25;

	projectile1PosX = 0;
	projectile1PosY = 0;
	projectile1Fired = false;

	projectile2PosX = 0;
	projectile2PosY = 0;
	projectile2Fired = false;

	// -------------- Health Bar --------------
	healthBarWidth = 7;
	healthBarHeight = 1;

	// Create a square
	float squareSide = 10;

	Mesh* squareTerrain = T1object2D::CreateSquare("squareTerrain", corner, squareSide, glm::vec3(0.04, 0.85, 0.27), true);
	AddMeshToList(squareTerrain);

	// Create TankBarrel
	Mesh* barrelTank = T1object2D::CreateBarrel("barrelTank", corner, 10, 7.5f, glm::vec3(0, 0, 0));
	AddMeshToList(barrelTank);

	// Create HealtBar
	Mesh* healthBarTankF = T1object2D::CreateBarrel("healthBarTankF", corner, 10, 7.5f, glm::vec3(1, 0, 0));
	AddMeshToList(healthBarTankF);

	Mesh* healthBarTankE = T1object2D::CreateSquare("healthBarTankE", corner, 10, glm::vec3(1, 1, 1), false);
	AddMeshToList(healthBarTankE);

	/// Create Tank1Body
	glm::vec3 tank1ColorBody = glm::vec3(1, 0.52, 0);
	glm::vec3 tank1ColorLink = glm::vec3(0.43, 0.27, 0.10);

	Mesh* bodyTank1 = T1object2D::CreateTankBody("bodyTank1", corner, 10, 14, 2, 3, tank1ColorBody, tank1ColorLink);
	AddMeshToList(bodyTank1);

	/// Create Tank2Body
	glm::vec3 tank2ColorBody = glm::vec3(0.14, 0.87, 0.55);
	glm::vec3 tank2ColorLink = glm::vec3(0.05, 0.27, 0.17);

	Mesh* bodyTank2 = T1object2D::CreateTankBody("bodyTank2", corner, 10, 14, 2, 3, tank2ColorBody, tank2ColorLink);
	AddMeshToList(bodyTank2);

	// Create a circle
	Mesh* circleTank1 = T1object2D::CreateCircle("circleTank1", corner, 5, tank1ColorBody);
	AddMeshToList(circleTank1);

	Mesh* circleTank2 = T1object2D::CreateCircle("circleTank2", corner, 5, tank2ColorBody);
	AddMeshToList(circleTank2);

	Mesh* circleBullet = T1object2D::CreateCircle("circleBullet", corner, 5, glm::vec3(0, 0, 0));
	AddMeshToList(circleBullet);
}

void Tank_Wars::UpdateTankPosition(float tankPosX, float* tankPosY, float* tankAngle)
{
	int seg_idx = tankPosX / interval;

	float Ax = seg_idx * interval;
	float Ay = std::get<1>(terrain_points[seg_idx]);
	float Bx = (seg_idx + 1) * interval;
	float By = std::get<1>(terrain_points[static_cast<std::vector<std::tuple<float, float>, std::allocator<std::tuple<float, float>>>::size_type>(seg_idx) + 1]);

	float t = (tankPosX - Ax) / (Bx - Ax);
	*tankPosY = Ay + t * (By - Ay);

	float dx = Bx - Ax;
	float dy = By - Ay;
	*tankAngle = atan2(dy, dx);
}

void Tank_Wars::CreateTerrain()
{
	// Create the terrain
	for (size_t i = 0; i < terrain_points.size() - 1; ++i) {
		auto& pointA = terrain_points[i];
		auto& pointB = terrain_points[i + 1];

		float distX = get<0>(pointB) - get<0>(pointA);
		float distY = get<1>(pointB) - get<1>(pointA);
		float ht = max(get<1>(pointA), get<1>(pointB));

		// Create square
		glm::mat3 modelMatrix = glm::mat3(1);
		modelMatrix *= T1transform2D::Translate(get<0>(pointA), get<1>(pointA));
		modelMatrix *= T1transform2D::Shear(distY / distX);
		modelMatrix *= T1transform2D::Scale(distX, ht);
		RenderMesh2D(meshes["squareTerrain"], shaders["VertexColor"], modelMatrix);

	}
}

void Tank_Wars::CreateTank(float tankPosX, float tankPosY, float tankAngle, float tankBarrelAngle, string tankType, tuple<float, float>* turretOffset, tuple<float, float>* barrelOffset)
{
	/*-------------- Body --------------*/
	glm::mat3 baseMatrix1 = glm::mat3(1);
	baseMatrix1 *= T1transform2D::Translate(tankPosX, tankPosY);
	baseMatrix1 *= T1transform2D::Rotate(tankAngle);
	baseMatrix1 *= T1transform2D::Scale(5, 5);
	RenderMesh2D(meshes["bodyTank" + tankType], shaders["VertexColor"], baseMatrix1);

	/*-------------- Turret --------------*/
	// Offset relative to tank body
	float circleOffsetX = 25;
	float circleOffsetY = 25;

	// Turret world position
	float turretOffsetX = circleOffsetX * cos(tankAngle) - circleOffsetY * sin(tankAngle);
	float turretOffsetY = circleOffsetX * sin(tankAngle) + circleOffsetY * cos(tankAngle);

	*turretOffset = make_tuple(turretOffsetX, turretOffsetY);

	glm::mat3 turretMatrix = glm::mat3(1);
	turretMatrix *= T1transform2D::Translate(tankPosX + turretOffsetX, tankPosY + turretOffsetY);
	turretMatrix *= T1transform2D::Scale(4, 4);
	RenderMesh2D(meshes["circleTank" + tankType], shaders["VertexColor"], turretMatrix);

	/*-------------- Barrel --------------*/
	// Offset relative to tank body
	float barrelOffsetX = 0;
	float barrelOffsetY = 0;

	// Barrel world position
	float barrelWorldX = barrelOffsetX * cos(tankAngle + tankBarrelAngle) - barrelOffsetY * sin(tankAngle + tankBarrelAngle);
	float barrelWorldY = barrelOffsetX * sin(tankAngle + tankBarrelAngle) + barrelOffsetY * cos(tankAngle + tankBarrelAngle);

	*barrelOffset = make_tuple(barrelWorldX, barrelWorldY);

	glm::mat3 barrelMatrix = glm::mat3(1);
	barrelMatrix *= T1transform2D::Translate(tankPosX + turretOffsetX + barrelWorldX, tankPosY + turretOffsetY + barrelWorldY);
	barrelMatrix *= T1transform2D::Rotate(tankBarrelAngle + tankAngle);
	barrelMatrix *= T1transform2D::Scale(4, 1);
	RenderMesh2D(meshes["barrelTank"], shaders["VertexColor"], barrelMatrix);
}

bool Tank_Wars::CheckCollision_Terrain_Screen(float projectileX, float projectileY)
{
	int seg_idx = static_cast<int>(projectileX / interval);

	float Ax = seg_idx * interval;
	float Ay = get<1>(terrain_points[seg_idx]);
	float Bx = (seg_idx + 1) * interval;
	float By = get<1>(terrain_points[static_cast<std::vector<std::tuple<float, float>, std::allocator<std::tuple<float, float>>>::size_type>(seg_idx) + 1]);

	float t = (projectileX - Ax) / (Bx - Ax);
	float y = Ay + t * (By - Ay);

	if ((projectileY - y) < collisionTol) {
		return true;
	}

	glm::ivec2 resolution = window->GetResolution();
	if (projectileX < 0 || projectileX > resolution.x || projectileY < 0 || projectileY > resolution.y) {
		return true;
	}

	return false;
}

bool Tank_Wars::CheckCollisionTank(float projectileX, float projectileY, float tankPosX, float tankPosY, float tankAngle)
{
	// SAT Separating Axis Theorem
	float tankWidth = 80;
	float tankHeight = 15;

	glm::vec2 tankCorners[] = {
		glm::vec2(0, 0),
		glm::vec2(0, tankHeight),
		glm::vec2(tankWidth, 0),
		glm::vec2(tankWidth, tankHeight)
	};

	glm::mat2 rotationMatrix = glm::mat2(
		glm::vec2(cos(tankAngle), -sin(tankAngle)),
		glm::vec2(sin(tankAngle), cos(tankAngle))
	);

	for (auto& tankCorner : tankCorners) {
		tankCorner = rotationMatrix * tankCorner + glm::vec2(tankPosX, tankPosY);
	}

	// Check if the projectile is inside the tank's bounding box using SAT
	glm::vec2 projectilePos = glm::vec2(projectileX, projectileY);

	for (int i = 0; i < 4; i++) {
		glm::vec2 edge = tankCorners[(i + 1) % 4] - tankCorners[i];
		glm::vec2 axis = glm::vec2(-edge.y, edge.x); // Perpendicular axis

		float minTank = FLT_MAX, maxTank = -FLT_MAX;
		for (int j = 0; j < 4; j++) {
			float projection = glm::dot(tankCorners[j], axis);
			minTank = std::min(minTank, projection);
			maxTank = std::max(maxTank, projection);
		}

		float projectileProjection = glm::dot(projectilePos, axis);
		if (projectileProjection < minTank || projectileProjection > maxTank) {
			return false; // No collision
		}
	}

	return true; // Collision detected
}

void Tank_Wars::DeformTerrain(vector<tuple<float, float>>* terrain_points, float projectilePosX, float projectilePosY, float radius)
{
	float sigma = radius / 2.0f; // Controls the spread of the deformation
	float maxDeformation = radius; // Maximum depth of the crater

	for (size_t i = 0; i < terrain_points->size(); ++i) {
		float pointX = get<0>((*terrain_points)[i]);
		float pointY = get<1>((*terrain_points)[i]);

		float dist = sqrt(pow((pointX - projectilePosX), 2) + pow((pointY - projectilePosY), 2));

		if (dist <= radius) {
			// Gaussian deformation
			float deformationAmount = exp(-dist * dist / (2 * sigma * sigma)) * maxDeformation;
			get<1>((*terrain_points)[i]) -= deformationAmount;
		}
	}
}

void Tank_Wars::IncrementalSmoothTerrain(vector<tuple<float, float>>* terrain_points, float deltaTimeSeconds)
{
	float smoothingFactor = 1.0f * deltaTimeSeconds; // Adjust as needed

	for (size_t i = 1; i < terrain_points->size() - 1; ++i) {
		float prevY = get<1>((*terrain_points)[i - 1]);
		float currY = get<1>((*terrain_points)[i]);
		float nextY = get<1>((*terrain_points)[i + 1]);

		float averageY = (prevY + nextY) / 2.0f;
		float deltaY = averageY - currY;

		// Apply a small fraction of the difference to smooth gradually
		get<1>((*terrain_points)[i]) += deltaY * smoothingFactor;
	}
}



void Tank_Wars::SlideTerrain(float deltaTimeSeconds, vector<tuple<float, float>>* terrain_points, float threshold) {
	// Smoothing factor (epsilon scaling)
	float epsilon = 20.0f * deltaTimeSeconds;

	// For each point in terrain_points
	for (size_t i = 0; i < terrain_points->size() - 1; ++i) {
		// Extract the heights of the current and next point
		float y1 = std::get<1>((*terrain_points)[i]);
		float y2 = std::get<1>((*terrain_points)[i + 1]);

		// Compute height difference
		float heightDifference = y2 - y1;

		// If the difference exceeds the threshold, apply smoothing
		if (fabs(heightDifference) > threshold) {
			// Calculate the transfer amount (epsilon)
			float transfer = epsilon * (heightDifference > 0 ? 1 : -1);

			// Adjust heights
			std::get<1>((*terrain_points)[i]) += transfer;
			std::get<1>((*terrain_points)[i + 1]) -= transfer;
		}
	}


}




void Tank_Wars::MoveProjectile(float deltaTimeSeconds, tuple<float, float>* movementProjectile1, float* projectilePosX, float* projectilePosY, float projectileAngle, float tankPosX, float tankPosY, float tankAngle)
{
	if (projectile1Fired) {
		// Update projectile position
		*projectilePosX += get<0>(*movementProjectile1) * deltaTimeSeconds;
		*projectilePosY += get<1>(*movementProjectile1) * deltaTimeSeconds;

		// Update projectile velocity due to gravity
		float gravity = 981.0f;
		get<1>(*movementProjectile1) -= gravity * deltaTimeSeconds;

		// Check for collision with terrain
		if (CheckCollision_Terrain_Screen(*projectilePosX, *projectilePosY)) {
			projectile1Fired = false;
			DeformTerrain(&terrain_points, *projectilePosX, *projectilePosY, radiusDeformation);
			//SmoothTerrain(&terrain_points, 10);
		}
		if (CheckCollisionTank(*projectilePosX, *projectilePosY, tankPosX, tankPosY, tankAngle)) {
			projectile1Fired = false;
			tank2Health -= projectileDamage;
			cout << "COLLISION: Tank2 Health: " << tank2Health << endl;
		}
	}
	if (projectile2Fired) {
		// Update projectile position
		*projectilePosX += get<0>(*movementProjectile1) * deltaTimeSeconds;
		*projectilePosY += get<1>(*movementProjectile1) * deltaTimeSeconds;

		// Update projectile velocity due to gravity
		float gravity = 981.0f;
		get<1>(*movementProjectile1) -= gravity * deltaTimeSeconds;

		// Check for collision with terrain
		if (CheckCollision_Terrain_Screen(*projectilePosX, *projectilePosY)) {
			projectile2Fired = false;
			DeformTerrain(&terrain_points, *projectilePosX, *projectilePosY, radiusDeformation);
			//SmoothTerrain(&terrain_points, 10);
		}
		if (CheckCollisionTank(*projectilePosX, *projectilePosY, tankPosX, tankPosY, tankAngle)) {
			projectile2Fired = false;
			tank1Health -= projectileDamage;
			cout << "COLLISION: Tank1 Health: " << tank1Health << endl;
		}
	}
}

void Tank_Wars::CreateProjectile(float projectilePosX, float projectilePosY, float projectileAngle)
{
	glm::mat3 projectileMatrix = glm::mat3(1);
	projectileMatrix *= T1transform2D::Translate(projectilePosX, projectilePosY);
	RenderMesh2D(meshes["circleBullet"], shaders["VertexColor"], projectileMatrix);
}

void Tank_Wars::CreateHealthBar(float tankPosX, float tankPosY, tuple<float, float> turretOffset, float tankHealth, float healthBarWidth, float healthBarHeight)
{
	float healthBarPosX = tankPosX + get<0>(turretOffset) - 40;
	float healthBarPosY = tankPosY + get<1>(turretOffset) + 50;

	glm::mat3 healthBarMatrixF = glm::mat3(1);
	healthBarMatrixF *= T1transform2D::Translate(healthBarPosX - 2, healthBarPosY - 5);
	healthBarMatrixF *= T1transform2D::Scale(healthBarWidth * (tankHealth / 100) * 1.1f, healthBarHeight + 0.5f);
	RenderMesh2D(meshes["healthBarTankF"], shaders["VertexColor"], healthBarMatrixF);

	glm::mat3 healthBarMatrixE = glm::mat3(1);
	healthBarMatrixE *= T1transform2D::Translate(healthBarPosX, healthBarPosY);
	healthBarMatrixE *= T1transform2D::Scale(healthBarWidth, healthBarHeight);
	RenderMesh2D(meshes["healthBarTankE"], shaders["VertexColor"], healthBarMatrixE);
}

void Tank_Wars::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0.40, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);

	CreateTerrain();
}

void Tank_Wars::Update(float deltaTimeSeconds)
{
	prevDeltaTimeSeconds = deltaTimeSeconds;

	// Update tank1 position
	if (tank1Health > 0) {
		UpdateTankPosition(tank1PosX, &tank1PosY, &tank1Angle);
		CreateTank(tank1PosX, tank1PosY, tank1Angle, tank1BarrelAngle, "1", &tank1TurretOffset, &tank1BarrelOffset);
		CreateHealthBar(tank1PosX, tank1PosY, tank1TurretOffset, tank1Health, healthBarWidth, healthBarHeight);
	}

	// Update tank2 position
	if (tank2Health > 0) {
		UpdateTankPosition(tank2PosX, &tank2PosY, &tank2Angle);
		CreateTank(tank2PosX, tank2PosY, tank2Angle, tank2BarrelAngle, "2", &tank2TurretOffset, &tank2BarrelOffset);
		CreateHealthBar(tank2PosX, tank2PosY, tank2TurretOffset, tank2Health, healthBarWidth, healthBarHeight);
	}

	// Update projectile position
	if (projectile1Fired) {
		MoveProjectile(deltaTimeSeconds, &movementProjectile1, &projectile1PosX, &projectile1PosY, projectile1Angle, tank2PosX, tank2PosY, tank2Angle);
		CreateProjectile(projectile1PosX, projectile1PosY, projectile1Angle);
	}

	// Update projectile position
	if (projectile2Fired) {
		MoveProjectile(deltaTimeSeconds, &movementProjectile2, &projectile2PosX, &projectile2PosY, projectile2Angle, tank1PosX, tank1PosY, tank1Angle);
		CreateProjectile(projectile2PosX, projectile2PosY, projectile2Angle);
	}

	SlideTerrain(deltaTimeSeconds, &terrain_points, slideTol);
	IncrementalSmoothTerrain(&terrain_points, deltaTimeSeconds);
}


void Tank_Wars::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tank_Wars::OnInputUpdate(float deltaTime, int mods)
{
	if (window->KeyHold(GLFW_KEY_A)) {
		if (tank1PosX > 15)
			tank1PosX -= tankSpeed;
	}
	if (window->KeyHold(GLFW_KEY_D)) {
		if (tank1PosX + 70 < window->GetResolution().x)
			tank1PosX += tankSpeed;
	}
	if (window->KeyHold(GLFW_KEY_W)) {
		if (tank1BarrelAngle < M_PI - 0.13) {
			tank1BarrelAngle += tankBarrelRotationSpeed;
		}
	}
	if (window->KeyHold(GLFW_KEY_S)) {
		if (tank1BarrelAngle > 0.13) {
			tank1BarrelAngle -= tankBarrelRotationSpeed;
		}
	}
	if (window->KeyHold(GLFW_KEY_LEFT)) {
		if (tank2PosX > 15)
			tank2PosX -= tankSpeed;
	}

	if (window->KeyHold(GLFW_KEY_RIGHT)) {
		if (tank2PosX + 70 < window->GetResolution().x)
			tank2PosX += tankSpeed;
	}

	if (window->KeyHold(GLFW_KEY_DOWN)) {
		if (tank2BarrelAngle < M_PI - 0.13)
			tank2BarrelAngle += tankBarrelRotationSpeed;
	}

	if (window->KeyHold(GLFW_KEY_UP)) {
		if (tank2BarrelAngle > 0.13)
			tank2BarrelAngle -= tankBarrelRotationSpeed;
	}
}


void Tank_Wars::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE && !projectile1Fired) {
		// Calculate the barrel's end position
		float barrelLength = 40.0f;
		float barrelEndX = tank1PosX + get<0>(tank1TurretOffset) + barrelLength * cos(tank1Angle + tank1BarrelAngle);
		float barrelEndY = tank1PosY + get<1>(tank1TurretOffset) + barrelLength * sin(tank1Angle + tank1BarrelAngle);

		// Set the projectile's initial position
		projectile1PosX = barrelEndX;
		projectile1PosY = barrelEndY;

		// Set the projectile's initial velocity
		float launchSpeed = 500.0f;
		float velocityX = launchSpeed * cos(tank1Angle + tank1BarrelAngle);
		float velocityY = launchSpeed * sin(tank1Angle + tank1BarrelAngle);
		movementProjectile1 = { velocityX, velocityY };

		projectile1Fired = true;
	}

	if (key == GLFW_KEY_ENTER && !projectile2Fired) {
		// Calculate the barrel's end position
		float barrelLength = 40.0f;
		float barrelEndX = tank2PosX + get<0>(tank2TurretOffset) + barrelLength * cos(tank2Angle + tank2BarrelAngle);
		float barrelEndY = tank2PosY + get<1>(tank2TurretOffset) + barrelLength * sin(tank2Angle + tank2BarrelAngle);

		// Set the projectile's initial position
		projectile2PosX = barrelEndX;
		projectile2PosY = barrelEndY;

		// Set the projectile's initial velocity
		float launchSpeed = 500.0f;
		float velocityX = launchSpeed * cos(tank2Angle + tank2BarrelAngle);
		float velocityY = launchSpeed * sin(tank2Angle + tank2BarrelAngle);
		movementProjectile2 = { velocityX, velocityY };

		projectile2Fired = true;
	}
}


void Tank_Wars::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Tank_Wars::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event
}


void Tank_Wars::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
}


void Tank_Wars::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Tank_Wars::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tank_Wars::OnWindowResize(int width, int height)
{
}
