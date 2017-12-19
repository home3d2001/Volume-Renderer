#pragma once

#include "Entities/Entity.h"
#include "Entities/GUI/Canvas.hpp"
#include "Entities/GUI/Button.hpp"
#include "Entities/GUI/CurveEditor/CurveEditor.hpp"
#include "Entities/GUI/2DEditor/2DEditor.hpp"
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
		void add2DEditor();
		void addButtons();

		std::shared_ptr<TwoDimensionEditor> editor;
		std::shared_ptr<Button> redCenterButton;
		std::shared_ptr<Button> greenCenterButton;
		std::shared_ptr<Button> blueCenterButton;
		std::shared_ptr<Button> alphaCenterButton;

		std::shared_ptr<Button> redSideButton;
		std::shared_ptr<Button> greenSideButton;
		std::shared_ptr<Button> blueSideButton;
		std::shared_ptr<Button> alphaSideButton;

		std::shared_ptr<Button> addWidgetButton;
		std::shared_ptr<Button> deleteWidgetButton;
		std::shared_ptr<Button> addPointButton;
		std::shared_ptr<Button> deletePointButton;
	
		std::shared_ptr<Texture> transferFunction;
		std::shared_ptr<Texture> histogramTexture;
		float aspectRatio;
		int bytesPerPixel;
	};
};