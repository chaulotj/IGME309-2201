#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	MakeCubeMesh(-3, 7);
	MakeCubeMesh(3, 7);
	MakeCubeMesh(-2, 6);
	MakeCubeMesh(2, 6);
	for (int c = -3; c < 4; c++) {
		MakeCubeMesh(c, 5);
	}
	for (int c = -4; c < 5; c++) {
		if (c != -2 && c != 2) {
			MakeCubeMesh(c, 4);
		}
	}
	for (int c = -5; c < 6; c++) {
		MakeCubeMesh(c, 3);
	}
	for (int c = -5; c < 6; c++) {
		if (c != -4 && c != 4) {
			MakeCubeMesh(c, 2);
		}
	}
	MakeCubeMesh(-5, 1);
	MakeCubeMesh(-3, 1);
	MakeCubeMesh(3, 1);
	MakeCubeMesh(5, 1);
	MakeCubeMesh(-2, 0);
	MakeCubeMesh(-1, 0);
	MakeCubeMesh(1, 0);
	MakeCubeMesh(2, 0);
}
void Application::MakeCubeMesh(int x, int y) 
{
	MyMesh* curMesh = new MyMesh();
	curMesh->GenerateCube(1.0f, C_BLACK);
	meshes.push_back(curMesh);
	positions.push_back(new vector2(x, y));
	meshNum++;
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	static float value = 0.0f;
	value += 0.01f;
	for (int c = 0; c < meshNum; c++)
	{
		meshes[c]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(value + (*positions[c]).x, (*positions[c]).y, 0.0f)));
	}
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);
	for (int c = 0; c < meshNum; c++) {
		SafeDelete(meshes[c]);
		SafeDelete(positions[c]);
	}
	//release GUI
	ShutdownGUI();
}