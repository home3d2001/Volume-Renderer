#include "TransferFunctionGUI.hpp"
#include "Entities/Box/Box.hpp"
#include "Options/Options.h"

namespace Entities {
	TransferFunctionGUI::TransferFunctionGUI(int width, int height, int bytesPerPixel) {
		using namespace std;
		
		this->aspectRatio = height / (float)width;
		this->bytesPerPixel = bytesPerPixel;
		
		/* Ignoring bytes per pixel at the moment */
		transferFunction = make_shared<Texture>(255, 1);

		/* Add a base canvas to work with. */
		addBaseCanvas();

		/* Add button for red, blue, green, and alpha */
		addButtons();
	}

	void TransferFunctionGUI::setHistogramTexture(std::shared_ptr<Texture> histogramTexture) {
		this->histogramTexture = histogramTexture;

		/* Add a curve editor. */
		addCurveEditor();
	}

	std::shared_ptr<Texture> TransferFunctionGUI::getTransferFunction() {
		return transferFunction;
	}

	void TransferFunctionGUI::addBaseCanvas() {
		std::shared_ptr<Canvas> canvas = make_shared<Canvas>();
		canvas->setColor(glm::vec4(1, 1, 1, .1));
		canvas->transform.SetScale(1.0 / 3.0, 1.0 / 2.0, 1.0);
		canvas->transform.SetPosition(-2.0 / 3.0, 0.5, 0.0);
		add("canvas", canvas);
	}

	void TransferFunctionGUI::addCurveEditor() {
		float curveEditorAspectRatio = aspectRatio * (.2 / .333);

		curveEditor = make_shared<CurveEditor>(transferFunction, histogramTexture, curveEditorAspectRatio);
		curveEditor->transform.SetScale(1.0 / 3.0, 1.0/3.0, 1.0);
		curveEditor->transform.SetPosition(-2.0 / 3.0, 2.0 / 3.0, 0.1);
		curveEditor->canvas->setColor(glm::vec4(1, 0, 0, 1));
		add("curveEditor", curveEditor);
	}

	void TransferFunctionGUI::addButtons() {
		redButton = make_shared<Button>();
		redButton->transform.SetScale((1.0 / 3.0) / 4.0, .1, .01);
		redButton->transform.SetPosition((-1 / 3.0) - (0 / 6.0) - (1 / 12.), .1, 0);
		add("redButton", redButton);

		blueButton = make_shared<Button>();
		blueButton->transform.SetScale((1.0 / 3.0) / 4.0, .1, .01);
		blueButton->transform.SetPosition((-1 / 3.0) - (1 / 6.0) - (1 / 12.), .1, 0);
		add("blueButton", blueButton);

		greenButton = make_shared<Button>();
		greenButton->transform.SetScale((1.0 / 3.0) / 4.0, .1, .01);
		greenButton->transform.SetPosition((-1 / 3.0) - (2 / 6.0) - (1 / 12.), .1, 0);
		add("greenButton", greenButton);

		alphaButton = make_shared<Button>();
		alphaButton->transform.SetScale((1.0 / 3.0) / 4.0, .1, .01);
		alphaButton->transform.SetPosition((-1 / 3.0) - (3 / 6.0) - (1 / 12.), .1, 0);
		add("alphaButton", alphaButton);

		/* Make buttons activatable */
		redButton->activatable = true;
		blueButton->activatable = true;
		greenButton->activatable = true;
		alphaButton->activatable = true;

		/* Setup button colors */
		redButton->color = glm::vec4(.2, 0, 0, 1);
		redButton->hoverColor = glm::vec4(0.6, 0, 0, 1);
		redButton->clickColor = glm::vec4(1.0, 0, 0, 1);

		greenButton->color = glm::vec4(0, .2, 0, 1);
		greenButton->hoverColor = glm::vec4(0, 0.6, 0, 1);
		greenButton->clickColor = glm::vec4(0, 1.0, 0, 1);

		blueButton->color = glm::vec4(0, 0, .2, 1);
		blueButton->hoverColor = glm::vec4(0, 0, 0.6, 1);
		blueButton->clickColor = glm::vec4(0, 0, 1.0, 1);

		alphaButton->color = glm::vec4(.2, .2, .2, 1);
		alphaButton->hoverColor = glm::vec4(0.6, 0.6, 0.6, 1);
		alphaButton->clickColor = glm::vec4(1.0, 1.0, 1.0, 1);
	}

	void TransferFunctionGUI::update() {
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_H) == GLFW_PRESS) {
			Options::showTransferFunctionGUI = false;
		}
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_T) == GLFW_PRESS) {
			Options::showTransferFunctionGUI = true;
		}
	}

	void TransferFunctionGUI::raycast(glm::vec4 p, glm::vec4 d) {
		if (!Options::showTransferFunctionGUI) return;

		Entity::raycast(p, d);

		/* Check if any of the buttons have been clicked */
		if (redButton->clicked) {
			blueButton->active = false;
			greenButton->active = false;
			alphaButton->active = false;
			curveEditor->setChannel(ColorChannel::RED);
		}

		if (blueButton->clicked) {
			redButton->active = false;
			greenButton->active = false;
			alphaButton->active = false;
			curveEditor->setChannel(ColorChannel::BLUE);
		}

		if (greenButton->clicked) {
			blueButton->active = false;
			redButton->active = false;
			alphaButton->active = false;
			curveEditor->setChannel(ColorChannel::GREEN);
		}

		if (alphaButton->clicked) {
			blueButton->active = false;
			greenButton->active = false;
			redButton->active = false;
			curveEditor->setChannel(ColorChannel::ALPHA);
		}

	}

	void TransferFunctionGUI::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		if (Options::showTransferFunctionGUI)
			Entity::render(parent_matrix, projection);
	}
}