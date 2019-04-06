#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Ian Matic - imm2853@g.rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}



	//LERP variables
	//Each orbit has its own currentIndex and restarting values
	for (int i = 0; i < m_uOrbits; i++) {

		currentIndex.push_back(0);
		restarting.push_back(false);

	}

	//calculate points in global space

	//Make each orbit
	for (int i = 0; i < m_uOrbits; i++) {
		std::vector<vector3> points;

		float twoPI = PI * 2;
		//each orbit has 3 + x (number orbit it is) points
		float divisions = 3 + i;

		//make the points in each orbit
		for (int j = 0; j < divisions; j++) {

			//each orbit is .5 bigger than the last
			float radius = 1 * (((float)i/2) + 1);

			float x = cosf(twoPI / divisions * j) * radius;
			float y = sinf(twoPI / divisions * j) * radius;
			float z = 0.0f;

			vector3 point = vector3(x, y, z);

			//add the point to the list of points
			points.push_back(point);
		}

		//add the list of points to the matrix
		pointMatrix.push_back(points);
	}

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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);


	/*Used for LERP*/
	//Get a timer
	static std::vector<float>fTimer;	//store the new timer
	static std::vector<uint> uClock;

	for (int i = 0; i < m_uOrbits; i++) {
		fTimer.push_back(0);
		uClock.push_back(m_pSystem->GenClock()); //generate a new clock for that timer
		fTimer[i] += m_pSystem->GetDeltaTime(uClock[i]); //get the delta time for that timer
	}

	
	// draw shapes
	//loop through each orbit
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		//calculate the current position
		vector3 v3CurrentPos;

		/*LERP CODE*/


		//your code goes here
		float xCalc;
		float yCalc;
		float zCalc;

		//going from last to first index
		if (restarting[i]) {

			//LERP
			v3CurrentPos = pointMatrix[i][pointMatrix[i].size() - 1] + fTimer[i] * (pointMatrix[i][0] - pointMatrix[i][pointMatrix[i].size() - 1]);

			xCalc = pow(pointMatrix[i][0].x - v3CurrentPos.x, 2);
			yCalc = pow(pointMatrix[i][0].y - v3CurrentPos.y, 2);
			zCalc = pow(pointMatrix[i][0].z - v3CurrentPos.z, 2);
		}
		else {

			//LERP
			v3CurrentPos = pointMatrix[i][currentIndex[i]] + fTimer[i] * (pointMatrix[i][currentIndex[i] + 1] - pointMatrix[i][currentIndex[i]]);

			xCalc = pow(pointMatrix[i][currentIndex[i] + 1].x - v3CurrentPos.x, 2);
			yCalc = pow(pointMatrix[i][currentIndex[i] + 1].y - v3CurrentPos.y, 2);
			zCalc = pow(pointMatrix[i][currentIndex[i] + 1].z - v3CurrentPos.z, 2);
		}



		//determine distance

		float distance = sqrtf(xCalc + yCalc + zCalc);

		if (distance < .01) {

			//finished restarting
			if (restarting[i]) {
				restarting[i] = false;
			}
			//don't set counter if just finished restarting
			else {
				//end of the vector
				if (currentIndex[i] == pointMatrix[i].size() - 2) {

					//go back to start
					currentIndex[i] = 0;
					restarting[i] = true;
				}
				else if (!restarting[i]) {

					currentIndex[i] += 1;
				}
			}


			//reset counter for new lerp
			fTimer[i] -= 1;

		}
		/*END LERP CODE*/



		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

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
	//release GUI
	ShutdownGUI();
}