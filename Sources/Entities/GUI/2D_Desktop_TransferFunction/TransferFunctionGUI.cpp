#include "TransferFunctionGUI.hpp"
#include "Entities/Box/Box.hpp"
#include "Options/Options.h"

namespace Entities {
	TransferFunctionGUI::TransferFunctionGUI(int width, int height, int bytesPerPixel) {
		using namespace std;
		
		this->aspectRatio = height / (float)width;
		this->bytesPerPixel = bytesPerPixel;
		
		/* Ignoring bytes per pixel at the moment */
		transferFunction = make_shared<Texture>(pow(2,12), pow(2,12)); // TODO: UPDATE THIS

		/* Add a base canvas to work with. */
		addBaseCanvas();

		/* Add button for red, blue, green, and alpha */
		addButtons();
	}

	void TransferFunctionGUI::setHistogramTexture(std::shared_ptr<Texture> histogramTexture) {
		this->histogramTexture = histogramTexture;

		/* Add a curve editor. */
		add2DEditor();
	}

	std::shared_ptr<Texture> TransferFunctionGUI::getTransferFunction() {
		return transferFunction;
	}

	void TransferFunctionGUI::addBaseCanvas() {
		std::shared_ptr<Canvas> canvas = make_shared<Canvas>();
		canvas->setColor(glm::vec4(1, 1, 1, .1));
		canvas->transform.SetScale(1.0 / 3.0, 1.0, 1.0);
		canvas->transform.SetPosition(-2.0 / 3.0, 0.0, 0.0);
		add("canvas", canvas);
	}

	void TransferFunctionGUI::add2DEditor() {
		float twoDimEditorAspRatio = aspectRatio * (.2 / .333); 

		/* TODO: SEND TF MIN MAX */
		editor = make_shared<TwoDimensionEditor>(transferFunction, histogramTexture, make_int2(0,0), make_int2(0,0), twoDimEditorAspRatio);
		editor->transform.SetScale(1.0 / 3.0, 1.0/2.0, 1.0);
		editor->transform.SetPosition(-2.0 / 3.0, 1.0 / 2.0, 0.2);
		editor->canvas->setColor(glm::vec4(1, 0, 0, 1));
		add("editor", editor);
	}

	void TransferFunctionGUI::addButtons() {
		float buttonYPos = -.5;

		redCenterButton = make_shared<Button>();
		redCenterButton->transform.SetScale((1.0 / 3.0) / 10.0, .1, .01);
		redCenterButton->transform.SetPosition((-1 / 3.0) - (0 / 14.0) - (1 / 28.), buttonYPos, .1);
		add("redButton", redCenterButton);

		blueCenterButton = make_shared<Button>();
		blueCenterButton->transform.SetScale((1.0 / 3.0) / 10.0, .1, .01);
		blueCenterButton->transform.SetPosition((-1 / 3.0) - (1 / 14.0) - (1 / 28.), buttonYPos, .1);
		add("blueButton", blueCenterButton);

		greenCenterButton = make_shared<Button>();
		greenCenterButton->transform.SetScale((1.0 / 3.0) / 10.0, .1, .01);
		greenCenterButton->transform.SetPosition((-1 / 3.0) - (2 / 14.0) - (1 / 28.), buttonYPos, .1);
		add("greenButton", greenCenterButton);

		alphaCenterButton = make_shared<Button>();
		alphaCenterButton->transform.SetScale((1.0 / 3.0) / 10.0, .1, .01);
		alphaCenterButton->transform.SetPosition((-1 / 3.0) - (3 / 14.0) - (1 / 28.), buttonYPos, .1);
		add("alphaButton", alphaCenterButton);


		redSideButton = make_shared<Button>();
		redSideButton->transform.SetScale((1.0 / 3.0) / 10.0, .1, .01);
		redSideButton->transform.SetPosition((-1 / 3.0) - (4 / 14.0) - (1 / 28.), buttonYPos, .1);
		add("redSideButton", redSideButton);

		blueSideButton = make_shared<Button>();
		blueSideButton->transform.SetScale((1.0 / 3.0) / 10.0, .1, .01);
		blueSideButton->transform.SetPosition((-1 / 3.0) - (5 / 14.0) - (1 / 28.), buttonYPos, .1);
		add("blueSideButton", blueSideButton);

		greenSideButton = make_shared<Button>();
		greenSideButton->transform.SetScale((1.0 / 3.0) / 10.0, .1, .01);
		greenSideButton->transform.SetPosition((-1 / 3.0) - (6 / 14.0) - (1 / 28.), buttonYPos, .1);
		add("greenSideButton", greenSideButton);

		alphaSideButton = make_shared<Button>();
		alphaSideButton->transform.SetScale((1.0 / 3.0) / 10.0, .1, .01);
		alphaSideButton->transform.SetPosition((-1 / 3.0) - (7 / 14.0) - (1 / 28.), buttonYPos, .1);
		add("alphaSideButton", alphaSideButton);


		addWidgetButton = make_shared<Button>();
		addWidgetButton->transform.SetScale((1.0 / 3.0) / 10.0, .07, .01);
		addWidgetButton->transform.SetPosition((-1 / 3.0) - (8.2 / 14.0) - (1 / 28.), buttonYPos +.3, .1);
		add("addWidgetButton", addWidgetButton);

		deleteWidgetButton = make_shared<Button>();
		deleteWidgetButton->transform.SetScale((1.0 / 3.0) / 10.0, .07, .01);
		deleteWidgetButton->transform.SetPosition((-1 / 3.0) - (8.2 / 14.0) - (1 / 28.), buttonYPos-.3, .1);
		add("deleteWidgetButton", deleteWidgetButton);

		/*addPointButton = make_shared<Button>();
		addPointButton->transform.SetScale((1.0 / 3.0) / 10.0, .07, .01);
		addPointButton->transform.SetPosition((-1 / 3.0) - (8.2 / 14.0) - (1 / 28.), buttonYPos + .1, .1);
		add("addPointButton", addPointButton);

		deletePointButton = make_shared<Button>();
		deletePointButton->transform.SetScale((1.0 / 3.0) / 10.0, .07, .01);
		deletePointButton->transform.SetPosition((-1 / 3.0) - (8.2 / 14.0) - (1 / 28.), buttonYPos+.3, .1);
		add("deletePointButton", deletePointButton);*/
		
		/*redCenterButton = make_shared<Button>();
		redCenterButton->transform.SetScale((1.0 / 3.0) / 4.0, .1, .01);
		redCenterButton->transform.SetPosition((-1 / 3.0) - (0 / 6.0) - (1 / 12.), buttonYPos, .1);
		add("redButton", redCenterButton);

		blueCenterButton = make_shared<Button>();
		blueCenterButton->transform.SetScale((1.0 / 3.0) / 4.0, .1, .01);
		blueCenterButton->transform.SetPosition((-1 / 3.0) - (1 / 6.0) - (1 / 12.), buttonYPos, .1);
		add("blueButton", blueCenterButton);

		greenCenterButton = make_shared<Button>();
		greenCenterButton->transform.SetScale((1.0 / 3.0) / 4.0, .1, .01);
		greenCenterButton->transform.SetPosition((-1 / 3.0) - (2 / 6.0) - (1 / 12.), buttonYPos, .1);
		add("greenButton", greenCenterButton);

		alphaCenterButton = make_shared<Button>();
		alphaCenterButton->transform.SetScale((1.0 / 3.0) / 4.0, .1, .01);
		alphaCenterButton->transform.SetPosition((-1 / 3.0) - (3 / 6.0) - (1 / 12.), buttonYPos, .1);
		add("alphaButton", alphaCenterButton);*/




		/* Setup button colors */
		redCenterButton->color = glm::vec4(1.0, 0, 0, 1);
		redCenterButton->hoverColor = glm::vec4(.9, 0, 0, 1);
		redCenterButton->clickColor = glm::vec4(.8, 0, 0, 1);

		greenCenterButton->color = glm::vec4(0, 1., 0, 1);
		greenCenterButton->hoverColor = glm::vec4(0, 0.9, 0, 1);
		greenCenterButton->clickColor = glm::vec4(0, .8, 0, 1);

		blueCenterButton->color = glm::vec4(0, 0, 1., 1);
		blueCenterButton->hoverColor = glm::vec4(0, 0, 0.9, 1);
		blueCenterButton->clickColor = glm::vec4(0, 0, 0.8, 1);

		alphaCenterButton->color = glm::vec4(1., 1., 1., 1);
		alphaCenterButton->hoverColor = glm::vec4(0.9, 0.9, 0.9, 1);
		alphaCenterButton->clickColor = glm::vec4(.8, .8, .8, 1);

		/* Setup button colors */
		redSideButton->color = glm::vec4(1.0, 0, 0, 1);
		redSideButton->hoverColor = glm::vec4(.9, 0, 0, 1);
		redSideButton->clickColor = glm::vec4(.8, 0, 0, 1);

		greenSideButton->color = glm::vec4(0, 1., 0, 1);
		greenSideButton->hoverColor = glm::vec4(0, 0.9, 0, 1);
		greenSideButton->clickColor = glm::vec4(0, .8, 0, 1);

		blueSideButton->color = glm::vec4(0, 0, 1., 1);
		blueSideButton->hoverColor = glm::vec4(0, 0, 0.9, 1);
		blueSideButton->clickColor = glm::vec4(0, 0, 0.8, 1);

		alphaSideButton->color = glm::vec4(1., 1., 1., 1);
		alphaSideButton->hoverColor = glm::vec4(0.9, 0.9, 0.9, 1);
		alphaSideButton->clickColor = glm::vec4(.8, .8, .8, 1);


		/* Setup button colors */
		addWidgetButton->color = glm::vec4(1., 1., 1., 1);
		addWidgetButton->hoverColor = glm::vec4(0.9, 0.9, 0.9, 1);
		addWidgetButton->clickColor = glm::vec4(.8, .8, .8, 1);

		deleteWidgetButton->color = glm::vec4(1., 1., 1., 1);
		deleteWidgetButton->hoverColor = glm::vec4(0.9, 0.9, 0.9, 1);
		deleteWidgetButton->clickColor = glm::vec4(.8, .8, .8, 1);

		/*addPointButton->color = glm::vec4(1., 1., 1., 1);
		addPointButton->hoverColor = glm::vec4(0.9, 0.9, 0.9, 1);
		addPointButton->clickColor = glm::vec4(.8, .8, .8, 1);

		deletePointButton->color = glm::vec4(1., 1., 1., 1);
		deletePointButton->hoverColor = glm::vec4(0.9, 0.9, 0.9, 1);
		deletePointButton->clickColor = glm::vec4(.8, .8, .8, 1);*/
	}

	void TransferFunctionGUI::update() {
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_H) == GLFW_PRESS) {
			Options::showTransferFunctionGUI = false;
		}
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_T) == GLFW_PRESS) {
			Options::showTransferFunctionGUI = true;
		}

		Entity::update();
	}

	void TransferFunctionGUI::raycast(glm::vec4 p, glm::vec4 d) {
		if (!Options::showTransferFunctionGUI) return;

		Entity::raycast(p, d);

		if (editor->widgetSelected) {

			vec4 widgetCenterColor = editor->getSelectedWidgetCenterColor();
			vec4 widgetSideColor = editor->getSelectedWidgetSideColor();


			redCenterButton->transform.SetPosition(vec3(redCenterButton->transform.position.x, widgetCenterColor.r - 1.0, .1));
			greenCenterButton->transform.SetPosition(vec3(greenCenterButton->transform.position.x, widgetCenterColor.g - 1.0, .1));
			blueCenterButton->transform.SetPosition(vec3(blueCenterButton->transform.position.x, widgetCenterColor.b - 1.0, .1));
			alphaCenterButton->transform.SetPosition(vec3(alphaCenterButton->transform.position.x, widgetCenterColor.a - 1.0, .1));


			redSideButton->transform.SetPosition(vec3(redSideButton->transform.position.x, widgetSideColor.r - 1.0, .1));
			greenSideButton->transform.SetPosition(vec3(greenSideButton->transform.position.x, widgetSideColor.g - 1.0, .1));
			blueSideButton->transform.SetPosition(vec3(blueSideButton->transform.position.x, widgetSideColor.b - 1.0, .1));
			alphaSideButton->transform.SetPosition(vec3(alphaSideButton->transform.position.x, widgetSideColor.a - 1.0, .1));

		}
		else {
			/* Check if any of the buttons have been held */
			if (redCenterButton->held) {
				if (p.y > 0.0) {
					redCenterButton->transform.SetPosition(vec3(redCenterButton->transform.position.x, -p.y, .1));
					editor->setSelectedWidgetCenterColor( glm::vec4(
						redCenterButton->transform.position.y + 1, 
						greenCenterButton->transform.position.y + 1, 
						blueCenterButton->transform.position.y + 1, 
						alphaCenterButton->transform.position.y +1));
				}
			}

			else if (blueCenterButton->held) {
				if (p.y > 0.0) {
					blueCenterButton->transform.SetPosition(vec3(blueCenterButton->transform.position.x, -p.y, .1));
					editor->setSelectedWidgetCenterColor(glm::vec4(
						redCenterButton->transform.position.y + 1,
						greenCenterButton->transform.position.y + 1,
						blueCenterButton->transform.position.y + 1,
						alphaCenterButton->transform.position.y + 1));
				}
			}

			else if (greenCenterButton->held) {
				if (p.y > 0.0) {
					greenCenterButton->transform.SetPosition(vec3(greenCenterButton->transform.position.x, -p.y, .1));
					editor->setSelectedWidgetCenterColor(glm::vec4(
						redCenterButton->transform.position.y + 1,
						greenCenterButton->transform.position.y + 1,
						blueCenterButton->transform.position.y + 1,
						alphaCenterButton->transform.position.y + 1));
				}
			}

			else if (alphaCenterButton->held) {
				if (p.y > 0.0) {
					alphaCenterButton->transform.SetPosition(vec3(alphaCenterButton->transform.position.x, -p.y, .1));
					editor->setSelectedWidgetCenterColor(glm::vec4(
						redCenterButton->transform.position.y + 1,
						greenCenterButton->transform.position.y + 1,
						blueCenterButton->transform.position.y + 1,
						alphaCenterButton->transform.position.y + 1));
				}
			}

			if (redSideButton->held) {
				if (p.y > 0.0) {
					redSideButton->transform.SetPosition(vec3(redSideButton->transform.position.x, -p.y, .1));
					editor->setSelectedWidgetSideColor(glm::vec4(
						redSideButton->transform.position.y + 1,
						greenSideButton->transform.position.y + 1,
						blueSideButton->transform.position.y + 1,
						alphaSideButton->transform.position.y + 1));
				}
			}

			else if (blueSideButton->held) {
				if (p.y > 0.0) {
					blueSideButton->transform.SetPosition(vec3(blueSideButton->transform.position.x, -p.y, .1));
					editor->setSelectedWidgetSideColor(glm::vec4(
						redSideButton->transform.position.y + 1,
						greenSideButton->transform.position.y + 1,
						blueSideButton->transform.position.y + 1,
						alphaSideButton->transform.position.y + 1));
				}
			}

			else if (greenSideButton->held) {
				if (p.y > 0.0) {
					greenSideButton->transform.SetPosition(vec3(greenSideButton->transform.position.x, -p.y, .1));
					editor->setSelectedWidgetSideColor(glm::vec4(
						redSideButton->transform.position.y + 1,
						greenSideButton->transform.position.y + 1,
						blueSideButton->transform.position.y + 1,
						alphaSideButton->transform.position.y + 1));
				}
			}

			else if (alphaSideButton->held) {
				if (p.y > 0.0) {
					alphaSideButton->transform.SetPosition(vec3(alphaSideButton->transform.position.x, -p.y, .1));
					editor->setSelectedWidgetSideColor(glm::vec4(
						redSideButton->transform.position.y + 1,
						greenSideButton->transform.position.y + 1,
						blueSideButton->transform.position.y + 1,
						alphaSideButton->transform.position.y + 1));
				}
			}


			if (addWidgetButton->clicked) {
				editor->addWidget();
			}

			if (deleteWidgetButton->clicked) {
				editor->deleteSelectedWidget();
			}
		}
	}

	void TransferFunctionGUI::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		if (Options::showTransferFunctionGUI)
			Entity::render(parent_matrix, projection);
		print_gl_error();
	}
}