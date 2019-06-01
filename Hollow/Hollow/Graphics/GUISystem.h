#pragma once
#include "Hollow/Graphics/GUI/ImGui/imgui.h"
#include "Hollow/Graphics/GUI/ImGui/imgui_impl_win32.h"
#include "Hollow/Graphics/GUI/ImGui/imgui_impl_dx11.h"
#include "Hollow/Graphics/GUI/ImGui/imgui_impl_opengl3.h"
#include "Renderer/DirectX/D3D11RenderApi.h"
#include "Renderer/DirectX/D3D11Context.h"
#include "Renderer/DirectX/D3D11Win32Window.h"
#include "Renderer/OpenGL/Win32/OGLWin32Window.h"
#include "Renderer/Base/Window.h"
#include "ShaderManager.h"
#include "Hollow/Common/FileSystem.h"
#include "Hollow/ECS/EntityManager.h"
#include "Hollow/ECS/GameObject.h"
#include "Hollow/ECS/RenderableComponent.h"
#include "Hollow/ECS/AnimationComponent.h"
#include "Hollow/ECS/TransformComponent.h"
#include "ForwardRenderSystem.h"
#include "Hollow/Graphics/Renderer/DirectX/D3D11Texture.h"
#include "Hollow/Graphics/Renderer/OpenGL/OGLTexture.h"
#include "Hollow/Graphics/Renderer/DirectX/D3D11RenderTarget.h"
#include "Hollow/Graphics/Renderer/OpenGL/OGLRenderTarget.h"
#include "TextureManager.h"

class GUISystem
{
public:
	bool open = true;
	std::string filename = "";
	GameObject* selectedGameObject;
	ForwardRenderSystem* renderSystem;
	Model* selectedModel;
	Material* selectedMaterial;

	GUISystem(Window* window, RenderApi* renderer)
	{
		bool result = true;
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		
#ifdef D3D11
		ImGui_ImplWin32_Init(*static_cast<D3D11Win32Window*>(window)->getHWND());
		D3D11Context& context = static_cast<D3D11RenderApi*>(renderer)->getContext();
		ImGui_ImplDX11_Init(context.getDevice(), context.getDeviceContext());
#elif ifdef OPENGL
		result = ImGui_ImplWin32_Init(*static_cast<OGLWin32Window*>(window)->getHWND());
		const char* glsl_version = "#version 460";
		result = ImGui_ImplOpenGL3_Init(glsl_version);
#endif
	}

	~GUISystem()
	{
#ifdef OPENGL
		ImGui_ImplOpenGL3_Shutdown();
#elif ifdef D3D11
		ImGui_ImplDX11_Shutdown();
#endif
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void begin()
	{
#ifdef OPENGL
		ImGui_ImplOpenGL3_NewFrame();
#elif ifdef D3D11
		ImGui_ImplDX11_NewFrame();
#endif
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Begin docking viewport
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowBgAlpha(0.0f);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &open, window_flags);
		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	void update(double dt)
	{
		begin();

		ImGui::Begin("Main");
		if (ImGui::TreeNode("Shaders")) {
			auto& shaders = ShaderManager::instance()->shaders;
			for (auto& it : shaders) {
				if (ImGui::TreeNode(it.first.c_str())) {
					std::string buttonName = "Reload##" + it.first;
					if (ImGui::Button(buttonName.c_str())) {
						ShaderManager::instance()->reloadShader(it.second);
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
#ifdef OPENGL
		ImGui::Image((void*)static_cast<OGLRenderTarget*>(renderSystem->target)->texture, ImVec2(100, 100));
#elif ifdef D3D11
		ImGui::Image(static_cast<D3D11RenderTarget*>(renderSystem->target)->GetShaderResourceView(), ImVec2(100, 100));
#endif
		if (ImGui::Button("Delete all textures")) {
			TextureManager::instance()->RemoveAll();
		}

		ImGui::Text(filename.c_str());

		int counter = 0;
		for (auto& entity : EntityManager::instance()->getContainer<GameObject>()->entityList) {
			if (ImGui::Selectable(("Entity" + std::to_string(counter++)).c_str())) {
				selectedGameObject = &entity;
			}
		}
		ImGui::End();

		ImGui::Begin("Inspector");
		ImGui::Text("Renderable component");
		if (selectedGameObject != nullptr) {
			if (selectedGameObject->hasComponent<RenderableComponent>()) {
				Mesh* mesh = selectedGameObject->getComponent<RenderableComponent>()->mesh;
				
				if (ImGui::TreeNode("Mesh")) {
					for (int i = 0; i < mesh->numModels; i++) {
						Model* model = mesh->models[i];
						if (ImGui::Selectable(mesh->models[i]->name.c_str())) {
							selectedModel = mesh->models[i];
							selectedMaterial = mesh->models[i]->material;
						}
					}
					ImGui::TreePop();
				}
			}
		}

		ImGui::Text("Transform component");
		if (selectedGameObject != nullptr) {
			if (selectedGameObject->hasComponent<TransformComponent>()) {
				TransformComponent* component = selectedGameObject->getComponent<TransformComponent>();
				ImGui::DragFloat3("Position", (float*)& component->position, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat3("Rotation", (float*)& component->rotation, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat3("Scale", (float*)& component->scale, 0.1f, -100.0f, 100.0f);
			}
		}
		ImGui::End();

		ImGui::Begin("Material properties");
		if (selectedMaterial != nullptr) {
			if (selectedMaterial->diffuse_texture != nullptr) {
#ifdef OPENGL
				ImGui::Image((void*)static_cast<OGLTexture*>(selectedMaterial->diffuse_texture)->textureId, ImVec2(100, 100));
#elif ifdef D3D11
				ImGui::Image(static_cast<D3D11Texture*>(selectedMaterial->diffuse_texture)->m_TextureShaderResource, ImVec2(100, 100));
#endif
				ImGui::SameLine();
			}

			if (ImGui::Button("Change")) {
				filename = Hollow::FileSystem::OpenFile("");
				if (filename.size()) {
					TextureManager::instance()->Remove(selectedMaterial->diffuse_texture);
					selectedMaterial->diffuse_texture = TextureManager::instance()->CreateTextureFromFile(filename, false);
				}
			}
		}

		ImGui::End();

		end();
	}

	void end()
	{
		ImGui::End();

		ImGui::Render();
#ifdef OPENGL
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
#ifdef ifdef D3D11 
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
};