#pragma once

#include "components/simple_scene.h"
#include <vector>
#include <tuple>
#include <string>

using std::vector;
using std::tuple;
using std::string;

namespace m1
{
	class Tank_Wars : public gfxc::SimpleScene
	{
	public:
		Tank_Wars();
		~Tank_Wars();

		void Init() override;

	private:
		void UpdateTankPosition(float tankPosX, float* tankPosY, float* tankAngle);
		void CreateTerrain();
		void CreateTank(float tankPosX, float tankPosY, float tankAngle, float tankBarrelAngle, string tankType, tuple<float, float>* turretOffset, tuple<float, float>* barrelOffset);
		void DeformTerrain(vector<tuple<float, float>>* terrain_points, float projectilePosX, float projectilePosY, float radius);
		void IncrementalSmoothTerrain(vector<tuple<float, float>>* terrain_points, float deltaTimeSeconds);
		void SlideTerrain(float deltaTimeSeconds, vector<tuple<float, float>>* terrain_points, float animationTol);
		void MoveProjectile(float deltaTimeSeconds, tuple<float, float>* movementProjectile1, float* projectilePosX, float* projectilePosY, float projectileAngle, float tankPosX, float tankPosY, float tankAngle);
		void CreateProjectile(float projectilePosX, float projectilePosY, float projectileAngle);
		bool CheckCollision_Terrain_Screen(float projectileX, float projectileY);
		bool CheckCollisionTank(float projectileX, float projectileY, float tankPosX, float tankPosY, float tankAngle);
		void CreateHealthBar(float tankPosX, float tankPosY, tuple<float, float> turretOffset, float tankHealth, float healthBarWidth, float healthBarHeight);
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

	protected:
		float cx, cy;
		glm::mat3 modelMatrix;
		vector<tuple<float, float>> terrain_points;
		float interval;
		float collisionTol, radiusDeformation;
		float prevDeltaTimeSeconds;

		// ----- Tank -----
		float tankSpeed, tankBarrelRotationSpeed;

		float tank1PosX, tank1PosY, tank1Angle, tank1BarrelAngle;
		tuple<float, float> tank1TurretOffset, tank1BarrelOffset;
		float tank1Health;

		float tank2PosX, tank2PosY, tank2Angle, tank2BarrelAngle;
		tuple<float, float> tank2TurretOffset, tank2BarrelOffset;
		float tank2Health;

		// ----- Projectile -----
		float projectileDamage;

		float projectile1PosX, projectile1PosY, projectile1Angle, projectile1Speed;
		tuple<float, float> movementProjectile1;
		bool projectile1Fired;

		float projectile2PosX, projectile2PosY, projectile2Angle, projectile2Speed;
		tuple<float, float> movementProjectile2;
		bool projectile2Fired;

		// ----- HealthBars -----
		float healthBarWidth, healthBarHeight;

		// ----- Slide Terrain -----
		float slideTol;
	};
}   // namespace m1
