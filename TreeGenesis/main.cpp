#include <string>
#include <sstream>
#include <math.h>
#include <ctime>

#include <iostream>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>

#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/UIEvents.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Skybox.h>

#include <Urho3D\Graphics\BillboardSet.h>
#include <Urho3D/Core/Context.h>
//////////////////////

#include "Tree.h"


using namespace Urho3D;
/**
* Using the convenient Application API we don't have
* to worry about initializing the engine or writing a main.
* You can probably mess around with initializing the engine
* and running a main manually, but this is convenient and portable.
*/
class MyApp : public Application
{
public:
	int framecount_;
	float time_;
	SharedPtr<Text> text_;
	SharedPtr<Scene> scene_;
	SharedPtr<Node> cameraNode_;
	SharedPtr<Window> window_;

	
	MyApp(Context * context) : Application(context), framecount_(0), time_(0)
	{
	}

	virtual void Setup()
	{
		engineParameters_["FullScreen"] = false;
		engineParameters_["WindowWidth"] = 1280;
		engineParameters_["WindowHeight"] = 720;
		engineParameters_["WindowResizable"] = true;
	}

	virtual void Start()
	{
		OpenConsoleWindow();

		ResourceCache* cache = GetSubsystem<ResourceCache>();

		//GUI
		initUI(context_);
		

		//RENDERING
		scene_ = new Scene(context_);
		scene_->CreateComponent<Octree>();


		Node* skyNode = scene_->CreateChild("Sky");
		//skyNode->SetScale(500.0f); // The scale actually does not matter
		Skybox* skybox = skyNode->CreateComponent<Skybox>();
		skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
		skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));

		
		cameraNode_ = scene_->CreateChild("Camera");
		cameraNode_->SetPosition(Vector3(-15, 40, -15));
		Camera* camera = cameraNode_->CreateComponent<Camera>();
		camera->SetFarClip(2000);


		{ // SunLight
			Node* lightNode = scene_->CreateChild();
			lightNode->SetDirection(Vector3::FORWARD);
			lightNode->Yaw(30);
			lightNode->Pitch(75);
			Light* light = lightNode->CreateComponent<Light>();
			light->SetLightType(LIGHT_DIRECTIONAL);
			light->SetBrightness(1.6);
			light->SetColor(Color(1.0, .99, 0.99, 1));
			light->SetCastShadows(true);
		}

		Tree::tParameters treeparams = {
			Tree::CrownType::BOX, 1500, {0, 20, 0}, {16,16,16},
			8.0f, 3.0f, 1.0f,
			{Tree::vec3f(0,0,0)},
			1, true, 0.2, 2.5, 4, 7,
			{50,50,50}
		};
		SharedPtr<Model> treemodel = Tree::getUrhoModelFromParams(context_, treeparams);
		
		Node* treenode = scene_->CreateChild("tree-node");
		StaticModel* treeobj = treenode->CreateComponent<StaticModel>();
		treeobj->SetModel(treemodel);
		treeobj->SetMaterial(cache->GetResource<Material>("Materials/TreeTrunk.xml"));
		treeobj->SetCastShadows(true);

		{ //save geometry to external file!
			PODVector<Drawable*> pod;
			pod.Push(treeobj);
			String filename = "output.obj";
			SharedPtr<File>file = SharedPtr<File>(new File(context_, filename, FILE_WRITE));
			WriteDrawablesToOBJ(pod, file, false, false);
			file->Close();
		}
		

		Node* boxnode = scene_->CreateChild("ground");
		boxnode->SetPosition({0,0,0});
		boxnode->SetScale({15,1,15});
		StaticModel* boxobj = boxnode->CreateComponent<StaticModel>();
		boxobj->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
		boxobj->SetMaterial(cache->GetResource<Material>("Materials/Dirt.xml"));
		boxobj->SetCastShadows(true);
		

		// Now we setup the viewport. Of course, you can have more than one!
		Renderer* renderer = GetSubsystem<Renderer>();
		SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
		renderer->SetViewport(0, viewport);


		SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MyApp, HandlePostRenderUpdate));
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MyApp, HandleKeyDown));
		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MyApp, HandleUpdate));
		SubscribeToEvent(E_UIMOUSECLICK, URHO3D_HANDLER(MyApp, HandleControlClicked));
	}


	virtual void Stop()
	{
	}


	void initUI(Context* context_) 
	{
		auto* cache = GetSubsystem<ResourceCache>();
		auto* graphics = GetSubsystem<Graphics>();
		auto* ui_root = GetSubsystem<UI>()->GetRoot();
		auto* ui_ = GetSubsystem<UI>();

		auto* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
		ui_root->SetDefaultStyle(style);

		context_->GetSubsystem<Urho3D::Input>()->SetMouseVisible(true);
		static Urho3D::Cursor* cursor = new Urho3D::Cursor(context_);
		cursor->SetShape(Urho3D::CursorShape::CS_IBEAM);

		//if(!GetSubsystem<Urho3D::UI>()->GetCursor())    // If I uncomment this it doesn't crash but does also nothing
		///////separate
		//GetSubsystem<Urho3D::UI>()->SetCursor(cursor);

		text_ = new Text(context_);
		text_->SetText("Keys: Mouse2 = rotate camera, Mouse2 + AWSD = move camera, Shift = fast mode, Esc = quit.\nWait a bit to see FPS.");
		text_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
		text_->SetColor(Color(0, 0, 0));
		text_->SetHorizontalAlignment(HA_LEFT);
		text_->SetVerticalAlignment(VA_TOP);
		ui_root->AddChild(text_);

		window_ = new Window(context_);
		ui_root->AddChild(window_);
		window_->SetMinWidth(384);
		window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
		//window_->SetAlignment(HA_LEFT, VA_CENTER);
		window_->SetPosition({ 10, 35 });
		window_->SetName("Window");
		window_->SetColor({0, 0, 0, 0.5});

		// Create Window 'titlebar' container
		auto* titleBar = new UIElement(context_);
		titleBar->SetMinSize(0, 24);
		titleBar->SetVerticalAlignment(VA_TOP);
		titleBar->SetLayoutMode(LM_HORIZONTAL);

		// Create the Window title Text
		auto* windowTitle = new Text(context_);
		windowTitle->SetName("WindowTitle");
		windowTitle->SetText("Hello GUI!");

		// Create the Window's close button
		auto* buttonClose = new Button(context_);
		buttonClose->SetName("CloseButton");

		// Add the controls to the title bar
		titleBar->AddChild(windowTitle);
		titleBar->AddChild(buttonClose);

		// Add the title bar to the Window
		window_->AddChild(titleBar);

		window_->SetStyleAuto();
		windowTitle->SetStyleAuto();
		buttonClose->SetStyle("CloseButton");
	}

	void HandleControlClicked(StringHash eventType, VariantMap& eventData) //handles clicks anywhere on the screen
	{
		// Get the Text control acting as the Window's title
		auto* windowTitle = window_->GetChildStaticCast<Text>("WindowTitle", true);

		// Get control that was clicked
		auto* clicked = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());

		String name = "...?";
		if (clicked)
		{
			// Get the name of the control that was clicked
			name = clicked->GetName();
		}

		// Update the Window's title text
		windowTitle->SetText("Hello " + name + "!");
	}

	
	void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
	{
		
	}

	void HandleKeyDown(StringHash eventType, VariantMap& eventData)
	{
		using namespace KeyDown;
		int key = eventData[P_KEY].GetInt();
		if (key == KEY_ESCAPE)
			engine_->Exit();

		/*if (key == KEY_TAB)    // toggle mouse cursor when pressing tab
		{
			GetSubsystem<Input>()->SetMouseVisible(!GetSubsystem<Input>()->IsMouseVisible());
			//GetSubsystem<Input>()->SetMouseGrabbed(!GetSubsystem<Input>()->IsMouseGrabbed());
		}*/
	}

	
	void HandleUpdate(StringHash eventType, VariantMap& eventData)
	{
		float timeStep = eventData[Update::P_TIMESTEP].GetFloat();
		framecount_++;
		time_ += timeStep;
		// Movement speed as world units per second
		float MOVE_SPEED = 10.0f;
		// Mouse sensitivity as degrees per pixel
		const float MOUSE_SENSITIVITY = 0.1f;

		if (time_ >= 3.14159 * 2)
		{
			std::string str;
			{
				std::ostringstream ss;
				ss << (float)framecount_ / time_;
				std::string s(ss.str());
				str.append(s.substr(0, 6));
			}
			str.append(" fps \nPlease hold Mouse2 to move and rotate the camera");
			String s(str.c_str(), str.size());
			text_->SetText(s);
			//URHO3D_LOGINFO(s);     // this shows how to put stuff into the log
			framecount_ = 0;
			time_ = 0;
		}


		auto* input = GetSubsystem<Input>();

		input->SetMouseVisible(!input->GetMouseButtonDown(4));

		if (input->GetMouseButtonDown(4)) { //m2 pressed
			if (input->GetQualifierDown(1))  // 1 is shift, 2 is ctrl, 4 is alt
				MOVE_SPEED *= 10;
			if (input->GetScancodeDown(26))
				cameraNode_->Translate(Vector3(0, 0, 1)*MOVE_SPEED*timeStep);
			if (input->GetScancodeDown(22))
				cameraNode_->Translate(Vector3(0, 0, -1)*MOVE_SPEED*timeStep);
			if (input->GetScancodeDown(4))
				cameraNode_->Translate(Vector3(-1, 0, 0)*MOVE_SPEED*timeStep);
			if (input->GetScancodeDown(7))
				cameraNode_->Translate(Vector3(1, 0, 0)*MOVE_SPEED*timeStep);
		}


		//if (!GetSubsystem<Input>()->IsMouseVisible())
		if (input->GetMouseButtonDown(4))
		{
			// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
			IntVector2 mouseMove = input->GetMouseMove();
			static float yaw_ = 45;
			static float pitch_ = 45;
			yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
			pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
			pitch_ = Clamp(pitch_, -90.0f, 90.0f);
			// Reset rotation and set yaw and pitch again
			cameraNode_->SetDirection(Vector3::FORWARD);
			cameraNode_->Yaw(yaw_);
			cameraNode_->Pitch(pitch_);
		}
	}
};

URHO3D_DEFINE_APPLICATION_MAIN(MyApp)