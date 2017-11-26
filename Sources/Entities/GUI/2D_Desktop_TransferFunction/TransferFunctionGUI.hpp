#pragma once

#include "Entities/Entity.h"
#include "Entities/GUI/Canvas.hpp"
#include "Entities/GUI/Button.hpp"
#include "Entities/GUI/CurveEditor/CurveEditor.hpp"
#include "Entities/ImagePlane/ImagePlane.hpp"

namespace Entities {
	class TransferFunctionGUI : public Entity {
	public:
		TransferFunctionGUI(int width, int height, int bytesPerPixel);
		void raycast(glm::vec4 p, glm::vec4 d);
		void update();
		void render(glm::mat4 parent_matrix, glm::mat4 projection);
		void setHistogramTexture(std::shared_ptr<Texture> histogramTexture);
		std::shared_ptr<Texture> getTransferFunction();

	private:
		void addBaseCanvas();
		void addCurveEditor();
		void addButtons();

		std::shared_ptr<CurveEditor> curveEditor;
		std::shared_ptr<Button> redButton;
		std::shared_ptr<Button> greenButton;
		std::shared_ptr<Button> blueButton;
		std::shared_ptr<Button> alphaButton;
		std::shared_ptr<Texture> transferFunction;
		std::shared_ptr<Texture> histogramTexture;
		float aspectRatio;
		int bytesPerPixel;
	};
};