#pragma once
#include "utils/logging.h"

static struct DrawFrameInfo {
	double acquireImage = 0.0f;
	double updateBuffers = 0.0f;
	double submitCommandBuffer = 0.0f;
	double presentation = 0.0f;
	double total = 0.0f;
};

const static void printDrawFrameInfo(const DrawFrameInfo& dfi, const std::string& from = "") noexcept {
	logging::loginfo("[%s] DrawFrameInfo:\n\
				Acquire Image:          %.4f ms\n\
				Update Buffers:         %.4f ms\n\
				Submit Command Buffer:  %.4f ms\n\
				Presentation:           %.4f ms\n\
				Total:                  %.4f ms",
		from.c_str(),
		dfi.acquireImage,
		dfi.updateBuffers,
		dfi.submitCommandBuffer,
		dfi.presentation,
		dfi.total);
}

static struct InitInfo {
	int width = 0;
	int height = 0;
	std::string name = "";
	double initWindow = 0.0f;
	double initVulkan = 0.0f;
	double total = 0.0f;
};

const static void printInitInfo(const InitInfo& ii, const std::string& from = "") noexcept {
	logging::loginfo("[%s] InitInfo:\n\
				Resolution:            %dx%d\n\
				Window Name:           %s\n\
				Init Window (glw):     %.4f ms\n\
				Init Vulkan:           %.4f ms\n\
				Total:                 %.4f ms",
		from.c_str(),
		ii.width, ii.height,
		ii.name.c_str(),
		ii.initWindow,
		ii.initVulkan,
		ii.total);
}
