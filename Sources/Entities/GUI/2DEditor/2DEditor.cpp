#include "2DEditor.hpp"

namespace Entities {
	TwoDimensionEditor::TwoDimensionEditor(std::shared_ptr<Texture> texture, shared_ptr<Texture> histogramTexture, int2 histTopLeft, int2 histBotRight, float aspectRatio) {
		this->aspectRatio = aspectRatio;
		this->texture = texture;

		canvas = make_shared<Canvas>();
		canvas->setColor(glm::vec4(1, 1, 1, .1));

		/* A 2D editor has a histogram  */
		std::shared_ptr<ImagePlane> histogram = make_shared<ImagePlane>(histogramTexture);
		histogram->setInterpolation(false);
		histogram->transform.SetScale(1.0, 1.0, 1.0);
		add("histogram", histogram);
		
		/* A 2D editor has a texture to render to */
		std::shared_ptr<ImagePlane> textureTarget = make_shared<ImagePlane>(texture);
		textureTarget->transform.SetScale(1.0, 1.0, 1.0);
		textureTarget->transform.SetPosition(0.0, 0.0, 0.1);
		add("textureTarget", textureTarget);

		/* A 2D editor as an indirect manipulation widget (Here, a pentagon )*/
	/*	widgets.push_back(make_shared<CircleWidget>());
		widgets[0]->setCenterColor(vec4(1, 0, 0, 1));
		widgets[0]->setSideColor(vec4(1, 0, 0, 0));
		widgets[0]->transform.SetPosition(0.0, 0.0, 0.2);
		add("widget" + widgets[0]->id, widgets[0]);*/

		/* A curve editor has an alternative frame buffer, which is used to render to the texture */
		initializeFrameBuffer();
	}

	//void CurveEditor::initializeTransferFunctionGeometry() {
	//	using namespace glm;

	//	/* Create a mesh of triangles. */
	//	/*
	//	X  X--X
	//	| \ \ |
	//	X--X  X
	//	*/

	//	/* Each "X" above is a vertex with a color, controlled by the editor.
	//	Vertically alligned  X's have the same color value. */


	//	/* Delta position between horizontal verticies*/
	//	float dx = 2.0 / (totalBoxes - 1);

	//	/* Initial x position in model space. */
	//	float x0 = -1.0;
	//	for (int i = 0; i < totalBoxes; ++i) {
	//		vec4 TL, BL, TR, BR;
	//		TL = vec4(x0 + i * dx, 1.0, 0.0, 1.0);
	//		BL = vec4(x0 + i * dx, -1.0, 0.0, 1.0);
	//		TR = vec4(x0 + (i + 1) * dx, 1.0, 0.0, 1.0);
	//		BR = vec4(x0 + (i + 1) * dx, -1.0, 0.0, 1.0);

	//		// T1: top left bottom left top right
	//		points.push_back(TL);
	//		points.push_back(BL);
	//		points.push_back(TR);

	//		// T2: top right bottom left bottom right
	//		points.push_back(TR);
	//		points.push_back(BL);
	//		points.push_back(BR);

	//		// random color for now
	//		vec4 C1 = vec4(i / (float)totalBoxes, i / (float)totalBoxes, i / (float)totalBoxes, i / (float)totalBoxes);
	//		i++;
	//		vec4 C2 = vec4(i / (float)totalBoxes, i / (float)totalBoxes, i / (float)totalBoxes, i / (float)totalBoxes);
	//		i--;
	//		colors.push_back(C1);
	//		colors.push_back(C1);
	//		colors.push_back(C2);
	//		colors.push_back(C2);
	//		colors.push_back(C1);
	//		colors.push_back(C2);
	//	}

	//	/* Upload this data to the GPU for rendering */
	//	updateVBO();
	//	updateVAO();
	//}

	void TwoDimensionEditor::initializeFrameBuffer() {
		/* A texture to render to */
		glBindTexture(GL_TEXTURE_2D, texture->textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		// Poor filtering. Needed !
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		print_gl_error();

		/* A custom frame buffer */
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		/* Depth buffer, because I don't know what I'm doing yet. */
		//glGenRenderbuffers(1, &depthBuffer);
		//glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture->width, texture->height);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		//print_gl_error();

		/* Finally, configure the frame buffer */
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->textureID, 0);
		glDrawBuffers(1, drawBuffers);
		print_gl_error();

		//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glClearColor(0, 0, 0, 1);

		//glEnable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
		////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_LINE_SMOOTH);
		//glEnable(GL_POLYGON_SMOOTH);
		//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);



		// Always check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR" << std::endl;
	}

	void TwoDimensionEditor::updateVAO() {
		///* attribute 1: vec4 point, attribute 2: vec4 color */
		//glDeleteVertexArrays(1, &VAO);
		//glGenVertexArrays(1, &VAO);
		//glBindVertexArray(VAO);

		///* Enable the point attribue */
		//glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		//glEnableVertexAttribArray(Shaders::lineProgram->position_id);
		//glVertexAttribPointer(Shaders::lineProgram->position_id, 4,
		//	GL_FLOAT, GL_FALSE, sizeof(float4), 0);
		//print_gl_error();

		///* Enable the color */
		//glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
		//glEnableVertexAttribArray(Shaders::lineProgram->color_id);
		//glVertexAttribPointer(Shaders::lineProgram->color_id, 4,
		//	GL_FLOAT, GL_FALSE, sizeof(float4), 0);
		//print_gl_error();
	}

	void TwoDimensionEditor::updateVBO() {
		


		//pointsVBOSize = sizeof(vec4) * points.size();
		//colorsVBOSize = sizeof(vec4) * colors.size();

		///* Upload the points */
		//print_gl_error();
		//glDeleteBuffers(1, &pointsVBO);
		//glGenBuffers(1, &pointsVBO);
		//glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		//print_gl_error();

		//glBufferData(GL_ARRAY_BUFFER, pointsVBOSize, points.data(), GL_STATIC_DRAW);
		//print_gl_error();

		///* Upload the colors */
		//glDeleteBuffers(1, &colorsVBO);
		//glGenBuffers(1, &colorsVBO);
		//glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
		//glBufferData(GL_ARRAY_BUFFER, colorsVBOSize, colors.data(), GL_STATIC_DRAW);
		//print_gl_error();

	}

	//void CurveEditor::addButtons() {
	//	float xstart = 1.0 - boxSize;
	//	float xend = -1.0 + boxSize;

	//	float ystart = .6 - boxSize;
	//	float yend = -1.0 + boxSize;

	//	/* Red boxes */
	//	for (int i = 0; i < totalBoxes; ++i) {
	//		float alpha = i / (float)(totalBoxes - 1);
	//		/* Add a button to the canvas */
	//		std::shared_ptr<Box> box = make_shared<Box>();
	//		box->transform.SetPosition(((xstart * alpha) + (xend * (1.0 - alpha))), ((ystart * alpha) + (yend * (1.0 - alpha))), .0001);
	//		box->transform.SetScale(boxSize, boxSize / aspectRatio, .0003);
	//		box->color = glm::vec4(1.0, 0.0, 0.0, 1.0);
	//		add("r" + std::to_string(i), box);
	//		redBoxes.push_back(box);
	//	}

	//	/* Blue boxes */
	//	for (int i = 0; i < totalBoxes; ++i) {
	//		float alpha = i / (float)(totalBoxes - 1);
	//		/* Add a button to the canvas */
	//		std::shared_ptr<Box> box = make_shared<Box>();
	//		box->transform.SetPosition(((xstart * alpha) + (xend * (1.0 - alpha))), ((ystart * alpha) + (yend * (1.0 - alpha))), .0001);
	//		box->transform.SetScale(boxSize, boxSize / aspectRatio, .0001);
	//		add("b" + std::to_string(i), box);
	//		box->color = glm::vec4(0.0, 0.0, 1.0, 1.0);
	//		blueBoxes.push_back(box);
	//	}

	//	/* Green boxes */
	//	for (int i = 0; i < totalBoxes; ++i) {
	//		float alpha = i / (float)(totalBoxes - 1);
	//		/* Add a button to the canvas */
	//		std::shared_ptr<Box> box = make_shared<Box>();
	//		box->transform.SetPosition(((xstart * alpha) + (xend * (1.0 - alpha))), ((ystart * alpha) + (yend * (1.0 - alpha))), .0001);
	//		box->transform.SetScale(boxSize, boxSize / aspectRatio, .0002);
	//		add("g" + std::to_string(i), box);
	//		box->color = glm::vec4(0.0, 1.0, 0.0, 1.0);
	//		greenBoxes.push_back(box);
	//	}

	//	/* Alpha boxes */
	//	for (int i = 0; i < totalBoxes; ++i) {
	//		float alpha = i / (float)(totalBoxes - 1);
	//		/* Add a button to the canvas */
	//		std::shared_ptr<Box> box = make_shared<Box>();
	//		box->transform.SetPosition(((xstart * alpha) + (xend * (1.0 - alpha))), ((ystart * alpha) + (yend * (1.0 - alpha))), .0001);
	//		box->transform.SetScale(boxSize, boxSize / aspectRatio, .0004);
	//		add("a" + std::to_string(i), box);
	//		box->color = glm::vec4(0.5, 0.5, 0.5, 1.0);
	//		alphaBoxes.push_back(box);
	//	}
	//}

	void TwoDimensionEditor::update() {
		Entity::update();
		for (int i = 0; i < widgets.size(); ++i) {
			if (widgets[i]->widgetHit) {
				widgets[i]->setEdgeColor(vec4(1.0, 1.0, 1.0, 1.0));
			}
			else {
				widgets[i]->setEdgeColor(vec4(.5, 0.5, 0.5, 1.0));
			}
		}
	}

	void TwoDimensionEditor::raycast(glm::vec4 p, glm::vec4 d) {
		Entity::raycast(p, d);
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

		if (state != GLFW_PRESS) {
			widgetSelected = false;
			boxSelected = false;
		}

		else {
			if (boxSelected) {
				widgets[selectedWidget]->setBoxPosition(-p.x, -p.y);
			}
			else if (widgetSelected) {
				widgets[selectedWidget]->transform.SetPosition(-p.x + offset.x, -p.y + offset.y, 0.2);
			}
			else {
				for (int i = 0; i < widgets.size(); ++i) {
					if (widgets[i]->boxSelected || widgets[i]->translatingWidget) {
						if (widgets[i]->boxSelected) {
							boxSelected = true;
							selectedWidget = i;
						}
						else if (widgets[i]->translatingWidget) {
							widgetSelected = true;
							selectedWidget = i;
							offset = widgets[selectedWidget]->transform.position - glm::vec3(-p.x, -p.y, 0.0);
						}
						break;
					}
				}
			}
		}
	}

	vec4 TwoDimensionEditor::getSelectedWidgetCenterColor() {
		return widgets[selectedWidget]->getCenterColor();
	}

	void TwoDimensionEditor::setSelectedWidgetCenterColor(vec4 color) {
		if (selectedWidget < widgets.size()) {
			widgets[selectedWidget]->setCenterColor(color);
		}
	}

	vec4 TwoDimensionEditor::getSelectedWidgetSideColor() {
		return widgets[selectedWidget]->getSideColor();
	}

	void TwoDimensionEditor::setSelectedWidgetSideColor(vec4 color) {
		if (selectedWidget < widgets.size()) {
			widgets[selectedWidget]->setSideColor(color);
		}
	}

	void TwoDimensionEditor::addWidget() {
		addingWidget = true;
		refresh = true;
	}

	void TwoDimensionEditor::deleteSelectedWidget() {
		deletingWidget = selectedWidget;
		selectedWidget--;
	}

	//void CurveEditor::setBoxPosition(int index, float yPos) {
	//	std::shared_ptr<Box> box;
	//	switch (editChannel) {
	//	case 0: box = redBoxes.at(index); break;
	//	case 2: box = blueBoxes.at(index); break;
	//	case 1: box = greenBoxes.at(index); break;
	//	case 3: box = alphaBoxes.at(index); break;
	//	}

	//	glm::vec3 newPosition = glm::vec3(box->transform.position.x, yPos, box->transform.position.z);

	//	if (newPosition.y > 0.6) newPosition.y = 0.6;
	//	if (newPosition.y < -1.0) newPosition.y = -1.0;

	//	box->transform.SetPosition(newPosition);

	//	setColor(index, (newPosition.y + 1.0) / 1.6);
	//}

	//void CurveEditor::setColor(int index, float intensity) {
	//	int i1 = 6 * index;
	//	int i2 = i1 + 1;
	//	int i3 = i1 + 4;

	//	colors.at(i1)[editChannel] = intensity;
	//	colors.at(i2)[editChannel] = intensity;
	//	colors.at(i3)[editChannel] = intensity;

	//	if (index != 0) {
	//		int i1 = (6 * (index - 1) + 2);
	//		int i2 = i1 + 1;
	//		int i3 = i1 + 3;

	//		colors.at(i1)[editChannel] = intensity;
	//		colors.at(i2)[editChannel] = intensity;
	//		colors.at(i3)[editChannel] = intensity;
	//	}

	//	refresh = true;
	//}

	//void CurveEditor::setChannel(ColorChannel channel) {
	//	switch (channel) {
	//	case ColorChannel::RED: editChannel = 0; break;
	//	case ColorChannel::GREEN: editChannel = 1; break;
	//	case ColorChannel::BLUE: editChannel = 2; break;
	//	case ColorChannel::ALPHA: editChannel = 3; break;
	//	}
	//}
	int testing = 0;
	void TwoDimensionEditor::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		if (deletingWidget != -1) {
			remove("widget" + std::to_string(widgets[deletingWidget]->id));
			widgets.erase(widgets.begin() + deletingWidget);
			deletingWidget = -1;
		}
		
		if (addingWidget) {
			std::shared_ptr<CircleWidget> widget = make_shared<CircleWidget>();
			widgets.push_back(widget);
			add("widget" + std::to_string(widget->id), widget);
			addingWidget = false;
			//glFinish();
		}

		if (refresh) {
			updateVBO();
			updateVAO();
			refresh = false;
		}

		/* Render to texture */
		print_gl_error();

		/* Bind alternative frame buffer */
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		//glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		print_gl_error();

		/* Constrain viewport to texture */
		glViewport(0, 0, texture->width, texture->height);
		print_gl_error();

		/* Draw widget geometry */
		for (int i = 0; i < widgets.size(); ++i) {
			widgets[i]->renderTriangles = true;
			widgets[i]->renderEdges = false;
			widgets[i]->renderBoxes = false;
			widgets[i]->render(glm::mat4(), glm::mat4());
			print_gl_error();
		}

		/* Switch back to the original frame buffer */
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glViewport(0, 0, GLUtilities::window_width, GLUtilities::window_height);
		print_gl_error();
		for (int i = 0; i < widgets.size(); ++i) {
			widgets[i]->renderTriangles = false;
			widgets[i]->renderEdges = true;
			widgets[i]->renderBoxes = true;
		}
		Entity::render(parent_matrix, projection);
	}
}