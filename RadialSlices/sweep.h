#pragma once
#include "clipper.hpp"

ClipperLib::Path sweep(ClipperLib::Path shape, ClipperLib::Path sweep_path);