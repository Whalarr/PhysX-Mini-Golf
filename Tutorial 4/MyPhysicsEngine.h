#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;
	

	//a list of colours: Circus Palette
	static const PxVec3 color_palette[] =
	{
		PxVec3(192.f/255.f,192.f/255.f,192.f/255.f),
		PxVec3(217.f/255.f,0.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,45.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,140.f/255.f,54.f/255.f),
		PxVec3(4.f/255.f,117.f/255.f,111.f/255.f),
		PxVec3(100.f / 255.f, 210.f / 255.f, 100.f / 255.f),
		PxVec3(79.f / 255.f, 115.f / 255.f, 2.f / 255.f),
		PxVec3(50.f / 255.f, 64.f / 255.f, 20.f / 255.f),
		PxVec3(69.f / 255.f, 100.f / 255.f, 38.f / 255.f),
		PxVec3(119.f / 255.f, 185.f / 255.f, 242.f / 255.f),
		PxVec3(204.f / 255.f, 102.f / 255.f, 0.f / 255.f),
		PxVec3(255.f / 255.f, 255.f / 255.f, 0.f / 255.f),
		PxVec3(50.f / 255.f, 205.f / 255.f, 50.f / 255.f)
	};


	//pyramid vertices
	static PxVec3 pyramid_verts[] = { PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1) };
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = { 1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1 };

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose = PxTransform(PxIdentity), PxReal density = 1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts), end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose = PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts), end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs), end(pyramid_trigs)), pose)
		{
		}
	};

	///An example class showing the use of springs (distance joints).
	class Trampoline
	{
		vector<DistanceJoint*> springs;
		Box *bottom, *top;
		PxMaterial* Bounce;

	public:
		Trampoline(PxVec3* dimensions, PxVec3 trampPos, PxReal stiffness = 0.2f, PxReal damping = 1.f)
		{
			PxVec3 TrampPos = trampPos;
			PxVec3* Dimensions = dimensions;
			PxReal thickness = .1f;
			Bounce = CreateMaterial(1.f, 1.f, 1.25f);

			bottom = new Box(PxTransform(PxVec3(TrampPos.x, thickness, TrampPos.z)), PxVec3(Dimensions->x, thickness, Dimensions->z));
			top = new Box(PxTransform(PxVec3(TrampPos.x, Dimensions->y + thickness / 2 , TrampPos.z)), PxVec3(Dimensions->x -0.1f, thickness, Dimensions->z - 0.1f));
			top->Material(Bounce);
			top->SetColor(color_palette[6]);
			//top->SetKinematic(true);
			top->Name("Tramp_top");

			springs.resize(4);
			springs[0] = new DistanceJoint(bottom, PxTransform(PxVec3(Dimensions->x, thickness, Dimensions->z)), top, PxTransform(PxVec3(Dimensions->x, -Dimensions->y / 2, Dimensions->z)));
			springs[1] = new DistanceJoint(bottom, PxTransform(PxVec3(Dimensions->x, thickness, -Dimensions->z)), top, PxTransform(PxVec3(Dimensions->x, -Dimensions->y/ 2, -Dimensions->z)));
			springs[2] = new DistanceJoint(bottom, PxTransform(PxVec3(-Dimensions->x, thickness, Dimensions->z)), top, PxTransform(PxVec3(-Dimensions->x, -Dimensions->y/2, Dimensions->z)));
			springs[3] = new DistanceJoint(bottom, PxTransform(PxVec3(-Dimensions->x, thickness, -Dimensions->z)), top, PxTransform(PxVec3(-Dimensions->x, -Dimensions->y/2, -Dimensions->z)));
			for (unsigned int i = 0; i < springs.size(); i++)
			{
				springs[i]->Stiffness(stiffness);
				springs[i]->Damping(damping);
			}

			bottom->SetKinematic(true);

		}

		void AddToScene(Scene* scene)
		{
			scene->Add(bottom);
			scene->Add(top);
		}

		~Trampoline()
		{
			for (unsigned int i = 0; i < springs.size(); i++)
				delete springs[i];
		}
	};

	class Track
	{
		DynamicActor* Track_Straight;
		
		PxMaterial* Rough;
		PxMaterial* Wall;

	public:
		
		DynamicActor* Track_Walls;
		PxVec3 TrackPos;
		
		Track(PxVec3* dimensions, PxVec3 trackPos, PxVec3 rotation, PxReal density = 1.f)
		{

			Rough = CreateMaterial(0.7f, 1.9f, 0.01f);
			Wall = CreateMaterial(0.7f, 1.9f, 0.01f);
			Wall->setRestitution(0.9f);
			

			PxReal thickness = 1.f;
			TrackPos = trackPos;
			PxVec3* Dimensions = dimensions;
			PxQuat Rotation;
			Rotation = PxQuat(1.0f, rotation);

			Track_Straight = new DynamicActor(PxTransform(TrackPos.x, thickness, TrackPos.z));
			Track_Walls = new DynamicActor(PxTransform(TrackPos.x, thickness, TrackPos.z));

			Track_Straight->CreateShape(PxBoxGeometry(Dimensions->x, thickness, Dimensions->z), 1.0f);

			Track_Walls->CreateShape(PxBoxGeometry(Dimensions->x / 6, thickness * 1.25, Dimensions->z),1.0f);
			Track_Walls->CreateShape(PxBoxGeometry(Dimensions->x / 6, thickness * 1.25, Dimensions->z), 1.0f);
			Track_Walls->GetShape(0)->setLocalPose(PxTransform(TrackPos.x - Dimensions->x / 1, thickness , 0.0f));
			Track_Walls->GetShape(1)->setLocalPose(PxTransform(TrackPos.x + Dimensions->x / 1, thickness , 0.0f));
			Track_Straight->SetKinematic(true);
			Track_Straight->Material(Rough);
			Track_Straight->SetColor(color_palette[8]);

			Track_Walls->SetKinematic(true);
			Track_Walls->Material(Wall);
			Track_Walls->SetColor(color_palette[10]);
		}

		void AddToScene(Scene* scene)
		{
			scene->Add(Track_Straight);
			scene->Add(Track_Walls);

		}
	};

	class ice
	{
		DynamicActor*Track_Straight;
		PxMaterial* ice_track;

	public:
		PxVec3 TrackPos;

		ice(PxVec3* dimensions, PxVec3 trackPos, PxReal density = 1.f)
		{
			ice_track = CreateMaterial(0.1f, 0.1f, 0.1f);
			PxReal thickness = 1.0f;
			TrackPos = trackPos;
			PxVec3* Dimensions = dimensions;

			Track_Straight = new DynamicActor(PxTransform(TrackPos.x, thickness, TrackPos.z));

			Track_Straight->CreateShape(PxBoxGeometry(Dimensions->x, thickness, Dimensions->z), 1.0f);

			Track_Straight->SetColor(color_palette[9]);
			Track_Straight->Material(ice_track);
			Track_Straight->SetKinematic(true);
		}
			
		void AddtoScene(Scene* scene)
		{
			scene->Add(Track_Straight);
		}
	};

	class Track_Corner
	{
		 
		PxMaterial* Rough;
		PxMaterial* Wall;

	public:
		DynamicActor* Track_Straight;
		PxVec3 TrackPos;
		DynamicActor* Track_Walls_corner;

		Track_Corner(PxVec3* dimensions, PxVec3 trackPos, PxQuat(Wall_rotation), PxReal density = 1.f)
		{
			Rough = CreateMaterial(0.7f, 1.9f, 0.01f);
			Wall = CreateMaterial(0.7f, 1.9f, 0.01f);
			Wall->setRestitution(1.f);

			PxReal thickness = 1.f;
			TrackPos = trackPos;
			PxVec3* Dimensions = dimensions;
			PxQuat corner_rot = PxQuat(PxPi / 4, PxVec3(.0f, -1.f, .0f));
			PxQuat Wall_rot = Wall_rotation;

			Track_Straight = new DynamicActor(PxTransform(TrackPos.x, thickness, TrackPos.z));
			Track_Walls_corner = new DynamicActor(PxTransform(TrackPos.x, thickness, TrackPos.z));

			Track_Straight->CreateShape(PxBoxGeometry(Dimensions->x, thickness, Dimensions->z), 1.0f);
			Track_Straight->SetColor(color_palette[8]);
			Track_Straight->SetKinematic(true);
			
			Track_Walls_corner->CreateShape(PxBoxGeometry(Dimensions->x / 6, thickness * 1.25, Dimensions->z),1.0f);
			Track_Walls_corner->CreateShape(PxBoxGeometry(Dimensions->x, thickness * 1.25, Dimensions->z / 6),1.0f);
			Track_Walls_corner->CreateShape(PxBoxGeometry(Dimensions->x / 8, thickness * 1.25, Dimensions->z / 2),1.0f);

			Track_Walls_corner->GetShape(0)->setLocalPose(PxTransform(TrackPos.x - Dimensions->x / 1, thickness , 0.0f, Wall_rot));
			Track_Walls_corner->GetShape(1)->setLocalPose(PxTransform(TrackPos.x, thickness, -Dimensions->z, Wall_rot));
			Track_Walls_corner->GetShape(2)->setLocalPose(PxTransform(TrackPos.x - Dimensions->x / 1.5, thickness, - Dimensions->z / 1.5, corner_rot * Wall_rot));
			

			Track_Walls_corner->SetKinematic(true);
			Track_Walls_corner->SetColor(color_palette[10]);
			
		}

		void AddtoScene(Scene* scene)
		{
			scene->Add(Track_Straight);
			scene->Add(Track_Walls_corner);
			
		}
	};

	class SinkHole
	{

		DynamicActor* Track_Sink;

		PxMaterial* Rough;
		PxMaterial* Wall;
	public:
		PxVec3 TrackPos;
		DynamicActor* Track_Walls;

		SinkHole(PxVec3* dimensions, PxVec3 trackPos, PxVec3 rotation, PxReal density = 1.f)
		{

			Rough = CreateMaterial(0.7f, 1.9f, 0.01f);
			Wall = CreateMaterial(0.7f, 1.9f, 0.01f);
			Wall->setRestitution(0.8f);


			PxReal thickness = 1.f;
			TrackPos = trackPos;
			PxVec3* Dimensions = dimensions;
			PxQuat Rotation;
			Rotation = PxQuat(1.0f, rotation);

			Track_Sink = new DynamicActor(PxTransform(TrackPos.x, thickness, TrackPos.z));
			Track_Walls = new DynamicActor(PxTransform(TrackPos.x, thickness, TrackPos.z));

			Track_Walls->CreateShape(PxBoxGeometry(Dimensions->x / 6, thickness * 1.25, Dimensions->z), 1.0f);
			Track_Walls->CreateShape(PxBoxGeometry(Dimensions->x / 6, thickness * 1.25, Dimensions->z), 1.0f);
			Track_Walls->CreateShape(PxBoxGeometry(Dimensions->x, thickness * 1.25, Dimensions->z / 6), 1.0f);
			Track_Walls->GetShape(0)->setLocalPose(PxTransform( - Dimensions->x / 1, thickness, 0.0f));
			Track_Walls->GetShape(1)->setLocalPose(PxTransform( Dimensions->x / 1, thickness, 0.0f));
			Track_Walls->GetShape(2)->setLocalPose(PxTransform(0.0f, thickness, Dimensions->z));


			Track_Walls->SetKinematic(true);
			Track_Walls->Material(Wall);
			Track_Walls->SetColor(color_palette[10]);

		
				Track_Sink->CreateShape(PxBoxGeometry(Dimensions->x / 3, thickness, Dimensions->z / 3), 1.0f);
				Track_Sink->CreateShape(PxBoxGeometry(Dimensions->x / 3, thickness, Dimensions->z / 3), 1.0f);
				Track_Sink->CreateShape(PxBoxGeometry(Dimensions->x / 3, thickness, Dimensions->z / 3), 1.0f);
				Track_Sink->CreateShape(PxBoxGeometry(Dimensions->x / 3, thickness, Dimensions->z / 3), 1.0f);
				Track_Sink->CreateShape(PxBoxGeometry(Dimensions->x / 3, thickness, Dimensions->z / 3), 1.0f);
				Track_Sink->CreateShape(PxBoxGeometry(Dimensions->x / 3, thickness, Dimensions->z / 3), 1.0f);
				Track_Sink->CreateShape(PxBoxGeometry(Dimensions->x / 3, thickness, Dimensions->z / 3), 1.0f);
				Track_Sink->CreateShape(PxBoxGeometry(Dimensions->x / 3, thickness, Dimensions->z / 3), 1.0f);
			
				Track_Sink->GetShape(0)->setLocalPose(PxTransform(TrackPos.x / 6, 0.0f, 0.0f ));
				Track_Sink->GetShape(1)->setLocalPose(PxTransform(-TrackPos.x / 6, 0.0f, 0.0f));
				Track_Sink->GetShape(2)->setLocalPose(PxTransform(0.0f, 0.0f, Dimensions->z / 1.5));
				Track_Sink->GetShape(3)->setLocalPose(PxTransform(0.0f, 0.0f, -Dimensions->z / 1.5));
				Track_Sink->GetShape(4)->setLocalPose(PxTransform(TrackPos.x / 6, 0.0f, Dimensions->z / 1.5));
				Track_Sink->GetShape(5)->setLocalPose(PxTransform(-TrackPos.x / 6, 0.0f, -Dimensions->z / 1.5));
				Track_Sink->GetShape(6)->setLocalPose(PxTransform(-TrackPos.x / 6, 0.0f, Dimensions->z / 1.5));
				Track_Sink->GetShape(7)->setLocalPose(PxTransform(TrackPos.x / 6, 0.0f, -Dimensions->z / 1.5));
				
				
			
				Track_Sink->SetKinematic(true);
				Track_Sink->Material(Rough);
				Track_Sink->SetColor(color_palette[8]);
		}

		void AddtoScene(Scene* scene)
		{
			scene->Add(Track_Sink);
			scene->Add(Track_Walls);

		}
	};

	class Windmill
	{
		DynamicActor* Windmill_body;
		DynamicActor* Windmill_blades;
		PxMaterial* Body_Mat;
		PxMaterial* Blade_Mat;

	public:


		Box* box;
		PxVec3 Windmill_pos;
		RevoluteJoint* joint;
		PxReal Motor_Speed = 1.f;


		Windmill(PxVec3* dimensions, PxVec3 windmill_pos, PxReal density = 1.0f)
		{
			Blade_Mat = CreateMaterial(1.0f, 1.0f, 1.8f);

			PxVec3* Dimensions = dimensions;
			Windmill_pos = windmill_pos;

			Windmill_body = new DynamicActor(PxTransform(Windmill_pos.x, Windmill_pos.y, Windmill_pos.z));
			Windmill_blades = new DynamicActor(PxTransform(Windmill_pos.x, Windmill_pos.y, Windmill_pos.z));

			Windmill_body->CreateShape(PxBoxGeometry(Dimensions->x, Dimensions->y, Dimensions->z), 1.0f);
			Windmill_body->CreateShape(PxBoxGeometry(Dimensions->x / 4, Dimensions->y, Dimensions->z), 1.0f);
			Windmill_body->CreateShape(PxBoxGeometry(Dimensions->x / 4, Dimensions->y, Dimensions->z), 1.0f);

			Windmill_body->GetShape(0)->setLocalPose(PxTransform(0.0f,Windmill_pos.y + Dimensions->y,0.0f));
			Windmill_body->GetShape(1)->setLocalPose(PxTransform(0.0f + Dimensions->x / 1, Windmill_pos.y, 0.0f));
			Windmill_body->GetShape(2)->setLocalPose(PxTransform(0.0f - Dimensions->x / 1, Windmill_pos.y, 0.0f));
			Windmill_body->SetKinematic(true);
			Windmill_body->SetColor(color_palette[1]);

			Windmill_blades->CreateShape(PxBoxGeometry(0.5f, 10.0f, 0.1f), 1.0f);
			Windmill_blades->CreateShape(PxBoxGeometry(0.5f, 10.0f, 0.1f), 1.0f);
			Windmill_blades->GetShape(0)->setLocalPose(PxTransform(PxVec3(0.f, 0.0f, 0.0f), PxQuat(PxPi / 2, PxVec3(1.0f, 0.0f, 0.0f))));
			Windmill_blades->GetShape(1)->setLocalPose(PxTransform(PxVec3(0.f, 0.0f, 0.0f), PxQuat(PxPi * 45, PxVec3(1.0f, 0.0f, 0.0f))));
			Windmill_blades->Material(Blade_Mat);
			Windmill_blades->SetColor(color_palette[10]);

			box = new Box(PxTransform(PxVec3(Windmill_pos.x, Windmill_pos.y + Dimensions->y, Windmill_pos.z + Dimensions->z - 0.5f)), PxVec3(1.0f,1.0f,1.0f), 1.0f);
			box->SetKinematic(true);

			/*
			//joint two boxes together
			//the joint is fixed to the centre of the first box, oriented by 90 degrees around the Y axis
			//and has the second object attached 5 meters away along the Y axis from the first object.*/
			joint = new RevoluteJoint(box, PxTransform(PxVec3(0.0f, Dimensions->y - (Dimensions->y/ 8), 1.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.f, 0.f))), Windmill_blades, PxTransform(PxVec3(0.0f, 0.f, 0.f)));
			joint->DriveVelocity(Motor_Speed);

			
			
		}
		

		void AddtoScene(Scene* scene)
		{
			scene->Add(Windmill_body);
			scene->Add(box);
			scene->Add(Windmill_blades);

		}

	};

	class Flipper
	{
		DynamicActor* Wheel_Spokes;
		DynamicActor* Supports;
		PxVec3 Wheel_pos;
		PxMaterial* Bounce;

	public:

		RevoluteJoint* joint;
		PxReal Motor_Speed = -1.f;


		Flipper(PxVec3 wheel_pos)
		{
			Wheel_pos = wheel_pos;
			Bounce = CreateMaterial(1.f, 1.f, 1.25f);

			Supports = new DynamicActor(PxTransform(Wheel_pos.x, Wheel_pos.y, Wheel_pos.z));
			Wheel_Spokes = new DynamicActor(PxTransform(Wheel_pos.x, Wheel_pos.y, Wheel_pos.z));
			
			Supports->CreateShape(PxBoxGeometry(0.5f, 5.0f,0.5f), 1.0f);
			Supports->CreateShape(PxBoxGeometry(0.5f, 5.0f, 0.5f), 1.0f);

			Supports ->GetShape(0)->setLocalPose(PxTransform(-8.0f, Wheel_pos.y, 0.0f));
			Supports->GetShape(1)->setLocalPose(PxTransform(8.0f,  Wheel_pos.y , 0.0f));
			Supports->SetKinematic(true);
			Supports->SetColor(color_palette[10]);

			Wheel_Spokes->CreateShape(PxBoxGeometry(8.f, 2.5f, 0.15f), 1.0f);
			Wheel_Spokes->SetColor(color_palette[1]);
			Wheel_Spokes->Material(Bounce);
			
			joint = new RevoluteJoint(Supports, PxTransform(PxVec3(0.0f, Wheel_pos.y, 0.0f), PxQuat(PxPi / 2, PxVec3(1.0f, 0.f, 0.f))),Wheel_Spokes, PxTransform(PxVec3(0.0f, 0.f, 0.f)));
			joint->DriveVelocity(Motor_Speed);

		}
		void AddtoScene(Scene* scene)
		{
			scene->Add(Wheel_Spokes);
			scene->Add(Supports);
			
		}
	};

	class Pushers
	{

		PxMaterial* Pusher_Bounce;
		PxVec3 Pusher_pos;
	public:
		DynamicActor* Pusher_Boxes;
		DynamicActor* Pusher_Trigger;

		Pushers(PxVec3 pusher_pos)
		{

			Pusher_Bounce = CreateMaterial(0.5f, 0.5f, 2.5f);
			Pusher_pos = pusher_pos;

			Pusher_Boxes = new DynamicActor(PxTransform(Pusher_pos.x, Pusher_pos.y, Pusher_pos.z));
			Pusher_Trigger = new DynamicActor(PxTransform(Pusher_pos.x, Pusher_pos.y, Pusher_pos.z));

			Pusher_Boxes->CreateShape(PxBoxGeometry(0.5f, 1.5f, 3.f), 100.0f);
			Pusher_Boxes->CreateShape(PxBoxGeometry(0.5f, 1.5f, 3.f), 100.0f);

			Pusher_Trigger->CreateShape(PxBoxGeometry(0.5f, 0.5f, 3.f), 100.0f);
			Pusher_Trigger->CreateShape(PxBoxGeometry(0.5f, 0.5f, 3.f), 100.0f);

			Pusher_Boxes->GetShape(0)->setLocalPose(PxTransform(3.5f, 0.55f, 5.25f));
			Pusher_Boxes->GetShape(1)->setLocalPose(PxTransform(-3.5f, 0.55f, -5.25f));
			Pusher_Boxes->Material(Pusher_Bounce);
			//Pusher_Boxes->SetKinematic(true);
			Pusher_Boxes->SetColor(color_palette[1]);

			Pusher_Trigger->GetShape(0)->setLocalPose(PxTransform(3.6f, 0.55f, 5.25f));
			Pusher_Trigger->GetShape(1)->setLocalPose(PxTransform(-3.4f, 0.55f, -5.25f));
			Pusher_Trigger->SetColor(color_palette[3]);
			Pusher_Trigger->SetKinematic(true);
			Pusher_Trigger->SetTrigger(true);
			Pusher_Trigger->Name("PUSHERS");
		}

		void AddtoScene(Scene* scene)
		{
			scene->Add(Pusher_Boxes);
			scene->Add(Pusher_Trigger);
		}
	};


	class FlagPole
	{
	public:
		Cloth* cloth;
		Box* Pole;
		PxVec3* Flag_pos;
	

		FlagPole(PxVec3* flag_pos)
		{
			
			Flag_pos = flag_pos;
		

			Pole = new Box(PxTransform(PxVec3(Flag_pos->x, Flag_pos->y, Flag_pos->z)), PxVec3(0.5f,10.0f,0.5f),1.0f);
			Pole->SetColor(color_palette[3]);
			Pole->SetKinematic(true);

			cloth = new Cloth(PxTransform(PxVec3(Flag_pos->x+ 0.5f, Flag_pos->y , Flag_pos->z + 0.5f), PxQuat(PxPi/2, PxVec3(0.0f,0.0f,1.0f))), PxVec2(8.f, 8.f), 40, 40);
			((PxCloth*)cloth->Get())->setExternalAcceleration(PxVec3(25.0f,0.0f,0.0f));
			
			cloth->SetColor(color_palette[2]);
			cloth->Name("FLAG");
			//setting custom cloth parameters
			//((PxCloth*)cloth->Get())->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(1.f));

		}
		void AddtoScene(Scene* scene)
		{
			scene->Add(Pole);
			scene->Add(cloth);
		}
		
	};

	class GolfClub
	{
		DynamicActor* Player_club;

	public:
		GolfClub()
		{
			Player_club = new DynamicActor(PxTransform(PxVec3(2.5f, 7.0f, 2.0f)));

			Player_club->CreateShape(PxBoxGeometry(PxVec3(0.5f, 5.0f, 0.5f)),1.0f);
			Player_club->CreateShape(PxBoxGeometry(PxVec3(3.0f, 0.75f, 0.5f)),1.0f);
			Player_club->GetShape(1)->setLocalPose(PxTransform(PxVec3(-1.0f, -4.25f, 0.0f)));
			Player_club->SetColor(color_palette[0]);
		}

		void AddtoScene(Scene*scene)
		{
			scene->Add(Player_club);
		}

	};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct FilterGroup
	{
		enum Enum
		{
			ACTOR0		= (1 << 0),
			ACTOR1		= (1 << 1),
			ACTOR2		= (1 << 2)
			//add more if you need
		};
	};

	///A customised collision class, implemneting various callbacks
	class MySimulationEventCallback : public PxSimulationEventCallback
	{
	public:
		
		//an example variable that will be checked in the main simulation loop
		bool trigger;
		bool Player_Win = false;
		bool Pusher_Hit = false;
		
		MySimulationEventCallback() : trigger(false) {}

		///Method called when the contact with the trigger object is detected.
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) 
		{
			//you can read the trigger information here
			for (PxU32 i = 0; i < count; i++)
			{
				//filter out contact with the planes
				if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
				{
					//check if eNOTIFY_TOUCH_FOUND trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						cerr << pairs[i].otherActor->getName() << endl;

						if (std::strcmp(pairs[i].otherActor->getName(), "Golf_Ball") == 0)
						{
							if (std::strcmp(pairs[i].triggerActor->getName(), "OUT") == 0)
							{
								pairs[i].otherActor->setGlobalPose(PxTransform(0.0f, 15.0f, 0.0f));
								pairs[i].otherActor->isRigidDynamic()->setLinearVelocity(PxVec3(0.0f,0.0f,0.0f));
								

								cerr << "BALL HAS FALLEN OUT OF BOUNDS!" << endl;
							}

							if (std::strcmp(pairs[i].triggerActor->getName(), "GOAL") == 0)
							{

								cerr << "PLAYER HAS REACHED THE GOAL!" << endl;
								
								Player_Win = true;

							}

							if (std::strcmp(pairs[i].triggerActor->getName(), "PUSHERS") == 0)
							{
								cerr << "PLAYER HAS HIT A PUSHER" << endl;
								Pusher_Hit = true;
							}

			
						}

						cerr << "onTrigger::eNOTIFY_TOUCH_FOUND" << endl;
						trigger = true;
					}
					//check if eNOTIFY_TOUCH_LOST trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						cerr << "onTrigger::eNOTIFY_TOUCH_LOST" << endl;
						trigger = false;
					}
				}
			}
		}

		///Method called when the contact by the filter shader is detected.
		virtual void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) 
		{
			cerr << "Contact found between " << pairHeader.actors[0]->getName() << " " << pairHeader.actors[1]->getName() << endl;

			//check all pairs
			for (PxU32 i = 0; i < nbPairs; i++)
			{
				//check eNOTIFY_TOUCH_FOUND
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					cerr << "onContact::eNOTIFY_TOUCH_FOUND" << endl;
				}
				//check eNOTIFY_TOUCH_LOST
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					cerr << "onContact::eNOTIFY_TOUCH_LOST" << endl;
				}
			}
		}

		virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {}
		virtual void onWake(PxActor **actors, PxU32 count) {}
		virtual void onSleep(PxActor **actors, PxU32 count) {}
	};

	//A simple filter shader based on PxDefaultSimulationFilterShader - without group filtering
	static PxFilterFlags CustomFilterShader( PxFilterObjectAttributes attributes0,	PxFilterData filterData0,
		PxFilterObjectAttributes attributes1,	PxFilterData filterData1,
		PxPairFlags& pairFlags,	const void* constantBlock,	PxU32 constantBlockSize)
	{
		// let triggers through
		if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		//enable continous collision detection
//		pairFlags |= PxPairFlag::eCCD_LINEAR;
		
		
		//customise collision filtering here
		//e.g.

		// trigger the contact callback for pairs (A,B) where 
		// the filtermask of A contains the ID of B and vice versa.
		if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			//trigger onContact callback for this pair of objects
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
//			pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}

		return PxFilterFlags();
	};

	///Custom scene class
	class MyScene : public Scene
	{
		PxMaterial* Sand_Mat;

		Plane* plane;
		Box* Tee, *Back_Wall, *Goal;

		Trampoline* tramp1;
		
		Track* Track1;
		Track* Track2;
		Track* Track3;
		
		
		Track_Corner* Corner_1;
		Track_Corner* Corner_2;
		Track_Corner* Corner_3;
		Track_Corner* Corner_4;

		ice* Ice_Track1;

		Windmill* windmill_1;
		Flipper* Wheel_1;
		Pushers* Pusher_1;

		SinkHole* Track_End;
		FlagPole* flag_1;

		GolfClub* Player_club;

	public:
		
		MySimulationEventCallback* my_callback;

		Sphere *Golf_Ball;
		PxReal Golf_ball_x = 0.f;
		PxReal Golf_ball_y = 15.0f;
		PxReal Golf_ball_z = 0.0f;
	
		//specify your custom filter shader here
		//PxDefaultSimulationFilterShader by default
		MyScene() : Scene() {};

		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

			//cloth visualisation
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_HORIZONTAL, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_VERTICAL, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_BENDING, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_SHEARING, 1.0f);

			//joints
			px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
			px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit()
		{
			SetVisualisation();

			GetMaterial()->setDynamicFriction(.2f);
			
			///Initialise and set the customised event callback
			my_callback = new MySimulationEventCallback();
			px_scene->setSimulationEventCallback(my_callback);

			//Create Sand
			Sand_Mat = CreateMaterial(1.6f, 1.6f, .1f);

			plane = new Plane();
			plane->SetColor(color_palette[7]);
			plane->SetTrigger(true);
			plane->Name("OUT");

			Add(plane);

			//TEE
			Tee = new Box(PxTransform(PxVec3(0.0f, 2.f, 0.0f)), PxVec3(1.0f, 0.25f, 1.0f),1.0f);
			Tee->SetColor(color_palette[5]);
			Tee->SetKinematic(true);
			Add(Tee);


			////GOLF BALL
			Golf_Ball = new Sphere(PxTransform(Golf_ball_x, Golf_ball_y, Golf_ball_z)), 1.0f, 3.0f;
			Golf_Ball->Name("Golf_Ball");

			//box->SetupFiltering(FilterGroup::ACTOR0, FilterGroup::ACTOR1);
			////use | operator to combine more actors e.g.
			//box->SetupFiltering(FilterGroup::ACTOR0, FilterGroup::ACTOR1 | FilterGroup::ACTOR2);
			////don't forget to set your flags for the matching actor as well, e.g.:
			//box2->SetupFiltering(FilterGroup::ACTOR1, FilterGroup::ACTOR0);

			Add(Golf_Ball);
			//SPAWN GOLF CLUB
			Player_club = new GolfClub();
			Player_club->AddtoScene(this);


			//SPAWN TRAMPOLINE
			tramp1 = new Trampoline(new PxVec3(10.0f, 10.0f, 10.0f), PxVec3(20.0f, 0.0f, -120.0f), 50.0f, 0.1f);
			tramp1->AddToScene(this);

			//SPAWN TRACKS
			Track1 = new Track(new PxVec3(10.0f, 3.0f, 30.0f), PxVec3(0.0f,3.0f,-20.0f), PxVec3(0.0f, 1.0f, 0.0f), 1.0f);
			Track1->AddToScene(this);

			////SPAWN TRACKS
			Track2 = new Track(new PxVec3(10.0f, 3.0f, 20.0f), PxVec3(20.0f, 3.0f, -90.0f), PxVec3(0.0f, 1.0f, 0.0f), 1.0f);
			Track2->Track_Walls->GetShape(0)->setLocalPose(PxTransform(PxVec3(10.0f, 0.24f, 0.0f)));
			Track2->Track_Walls->GetShape(1)->setLocalPose(PxTransform(PxVec3(-10.0f, 0.24f, 0.0f)));
			Track2->AddToScene(this);

			Track3 = new Track(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(0.0f, 3.0f, -140.0f), PxVec3(0.0f, 1.0f, 0.0f), 1.0f);
			Track3->Track_Walls->GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, 0.24f, 10.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Track3->Track_Walls->GetShape(1)->setLocalPose(PxTransform(PxVec3(0.0f, 0.24f, -10.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Track3->AddToScene(this);


			Corner_1 = new Track_Corner(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(0.0f, 3.0f, -60.0f), PxQuat(PxPi * 2, PxVec3(0.0f,1.0f,0.0f)), 1.0f);
			Corner_1->Track_Straight->Material(Sand_Mat);
			Corner_1->Track_Straight->SetColor(color_palette[11]);
			Corner_1->AddtoScene(this);

			Corner_2 = new Track_Corner(new PxVec3(10.0f, 1.0f, 10.0f), PxVec3(20.0f, 3.0f, -60.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f)), 1.0f);		
			Corner_2->Track_Walls_corner->GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, 0.24f, 10.0f), PxQuat(PxPi / 2, PxVec3(0.0f,1.0f,0.0f))));
			Corner_2->Track_Walls_corner->GetShape(1)->setLocalPose(PxTransform(PxVec3(10.0f, 0.24f, 0.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Corner_2->Track_Walls_corner->GetShape(2)->setLocalPose(PxTransform(PxVec3(6.0f, 0.24f, 6.0f), PxQuat(PxPi / 4, PxVec3(0.0f, -1.0f, 0.0f))));
			Corner_2->AddtoScene(this);

			Corner_3 = new Track_Corner(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(20.0f, 3.0f, -140.0f), PxQuat(PxPi * 2, PxVec3(0.0f, 1.0f, 0.0f)), 1.0f);
			Corner_3->Track_Walls_corner->GetShape(1)->setLocalPose(PxTransform(PxVec3(0.0f, 0.24f, -10.0f), PxQuat(PxPi * 2, PxVec3(0.0f, 0.0f, 0.0f))));
			Corner_3->Track_Walls_corner->GetShape(2)->setLocalPose(PxTransform(PxVec3(6.0f, 0.24f, -6.0f), PxQuat(PxPi / 4, PxVec3(0.0f, 1.0f, 0.0f))));
			Corner_3->AddtoScene(this);

			Corner_4 = new Track_Corner(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(-40.0f, 3.0f, -140.0f), PxQuat(PxPi * 2, PxVec3(0.0f, 1.0f, 0.0f)), 1.0f);
			Corner_4->Track_Walls_corner->GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, 0.24f, -10.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Corner_4->Track_Walls_corner->GetShape(1)->setLocalPose(PxTransform(PxVec3(-10.0f, 0.24f, 0.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Corner_4->Track_Walls_corner->GetShape(2)->setLocalPose(PxTransform(PxVec3(-6.0f, 0.24f, -6.0f), PxQuat(PxPi / 4, PxVec3(0.0f, -1.0f, 0.0f))));
			Corner_4->AddtoScene(this);

			Ice_Track1 = new ice(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(-20.0f, 3.0f, -140.0f), 1.0f);
			Ice_Track1->AddtoScene(this);
			

			Back_Wall = new Box(PxTransform(PxVec3(0.0f,0.0f,10.0f)), PxVec3(10.0f,3.2f,1.0f), 1.0f);
			Back_Wall->SetColor(color_palette[10]);
			Back_Wall->SetKinematic(true);
			Add(Back_Wall);

			//SPAWN WINDMILLS
			windmill_1 = new Windmill(new PxVec3(5.0f, 5.0f, 5.0f), PxVec3(20.0f, 3.0f, -80.0f));
			windmill_1->AddtoScene(this);

			Wheel_1 = new Flipper(PxVec3(0.0f, 3.0f, -20.0f));
			Wheel_1->AddtoScene(this);

			Pusher_1 = new Pushers(PxVec3(0.0f, 3.0f, -140.0f));
			Pusher_1->AddtoScene(this);

			Track_End = new SinkHole(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(-40.0f, 3.0f, -120.0f), PxVec3(0.0f, 1.0f, 0.0f), 1.0f);
			Track_End->AddtoScene(this);

			flag_1 = new FlagPole(new PxVec3(-40.0f,15.0f,-120.0f));
			flag_1->AddtoScene(this);
			
			Goal = new Box(PxTransform(PxVec3(-40.0f, 1.25f, -120.0f)), PxVec3(3.5f, 0.15f, 3.5f), 1.0f);
			Goal->SetColor(color_palette[5]);
			Goal->SetKinematic(true);
			Goal->SetTrigger(true);
			Goal->Name("GOAL");
			Add(Goal);

			//int objects = 120;
			//for (int i = 0; i < objects; i++)
			//{
			//	////GOLF BALL
			//	Golf_Ball = new Sphere(PxTransform(Golf_ball_x, Golf_ball_y, Golf_ball_z)), 10.0f, 3.0f;
			//	
			//	Add(Golf_Ball);
			//}


		}
		//Custom udpate function
		virtual void CustomUpdate() 
		{

			
		//	cerr << my_callback->Player_Win << endl;

			if (my_callback->Player_Win == true)
			{
				flag_1->cloth->SetColor(color_palette[12]);
			}

			if (my_callback->Pusher_Hit == true)
			{
				
				Pusher_1->Pusher_Boxes->SetColor(color_palette[4]);
				my_callback->Pusher_Hit = false;
			}
		}

		/// An example use of key release handling
		void ExampleKeyReleaseHandler()
		{
			cerr << "I am realeased!" << endl;
		}

		/// An example use of key presse handling
		void ExampleKeyPressHandler()
		{
			cerr << "I am pressed!" << endl;
		}
	};
}
