/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          http://www.mrpt.org/                          |
   |                                                                        |
   | Copyright (c) 2005-2018, Individual contributors, see AUTHORS file     |
   | See: http://www.mrpt.org/Authors - All rights reserved.                |
   | Released under BSD License. See details in http://www.mrpt.org/License |
   +------------------------------------------------------------------------+ */

#include <mrpt/img/CImage.h>
#include <mrpt/img/TColor.h>
#include <mrpt/math/CMatrixTemplateNumeric.h>
#include <CTraitsTest.h>
#include <gtest/gtest.h>
#include <test_mrpt_common.h>

// Universal include for all versions of OpenCV
#include <mrpt/otherlibs/do_opencv_includes.h>

template class mrpt::CTraitsTest<mrpt::img::CImage>;

using namespace std::string_literals;
const auto tstImgFileColor =
    mrpt::UNITTEST_BASEDIR + "/samples/img_basic_example/frame_color.jpg"s;

TEST(CImage, CtorDefault)
{
	mrpt::img::CImage img;
	EXPECT_THROW(img.getWidth(), std::exception);
}

#if MRPT_HAS_OPENCV

TEST(CImage, CtorSized)
{
	using namespace mrpt::img;
	{
		CImage img(64, 48, CH_RGB);
		EXPECT_EQ(img.getWidth(), 64U);
		EXPECT_EQ(img.getHeight(), 48U);
		EXPECT_EQ(img.getChannelCount(), 3U);
		EXPECT_EQ(img.getPixelDepth(), PixelDepth::D8U);
		EXPECT_TRUE(img.isColor());
	}
	{
		CImage img(64, 48, CH_GRAY);
		EXPECT_EQ(img.getWidth(), 64U);
		EXPECT_EQ(img.getHeight(), 48U);
		EXPECT_EQ(img.getChannelCount(), 1U);
		EXPECT_EQ(img.getPixelDepth(), PixelDepth::D8U);
		EXPECT_FALSE(img.isColor());
	}
}

TEST(CImage, GetSetPixel)
{
	using namespace mrpt::img;
	CImage img(20, 10, CH_GRAY);
	img.setPixel(10, 2, TColor(0x80, 0x80, 0x80));
	EXPECT_EQ(img.at<uint8_t>(10, 2), 0x80);

	img.setPixel(11, 2, TColor(0x0, 0x0, 0x0));
	EXPECT_EQ(img.at<uint8_t>(11, 2), 0x00);

	img.setPixel(12, 2, TColor(0xff, 0xff, 0xff));
	EXPECT_EQ(img.at<uint8_t>(12, 2), 0xff);

	img.at<uint8_t>(13, 2) = 0x70;
	EXPECT_EQ(img.at<uint8_t>(13, 2), 0x70);

	auto* line = img.ptrLine<uint8_t>(5);
	for (uint8_t i = 0; i < 20; i++)
	{
		line[i] = i;
		EXPECT_EQ(img.at<uint8_t>(i, 5), i);
	}

	mrpt::math::CMatrixFloat M;
	img.getAsMatrix(M, true, 0, 0, -1, -1, false /* dont normalize (0,1) */);
	for (uint8_t i = 0; i < 20; i++)
	{
		EXPECT_NEAR(static_cast<double>(M(5, i)), i, 1e-8);
	}
}

TEST(CImage, CopyMoveSwap)
{
	using namespace mrpt::img;
	{
		CImage a(20, 10, CH_GRAY);
		a.at<uint8_t>(1, 2) = 0x80;
		// Shallow copy:
		CImage b = a;
		EXPECT_EQ(b.at<uint8_t>(1, 2), 0x80);

		a.at<uint8_t>(1, 3) = 0x81;
		EXPECT_EQ(b.at<uint8_t>(1, 3), 0x81);

		// Deep copy:
		CImage c = a.makeDeepCopy();
		EXPECT_EQ(c.at<uint8_t>(1, 2), 0x80);

		c.at<uint8_t>(1, 3) = 0x0;
		a.at<uint8_t>(1, 3) = 0x81;
		EXPECT_NE(c.at<uint8_t>(1, 3), 0x81);
	}

	{
		CImage a(20, 10, CH_GRAY);
		a.at<uint8_t>(1, 2) = 0x80;
		// Shallow copy ctor:
		CImage b(a, mrpt::img::SHALLOW_COPY);
		EXPECT_EQ(b.at<uint8_t>(1, 2), 0x80);

		a.at<uint8_t>(1, 3) = 0x81;
		EXPECT_EQ(b.at<uint8_t>(1, 3), 0x81);

		// Deep copy ctor:
		CImage c(a, mrpt::img::DEEP_COPY);
		EXPECT_EQ(c.at<uint8_t>(1, 2), 0x80);

		c.at<uint8_t>(1, 3) = 0x0;
		a.at<uint8_t>(1, 3) = 0x81;
		EXPECT_NE(c.at<uint8_t>(1, 3), 0x81);
	}

	{
		CImage a(20, 10, CH_GRAY);
		a.at<uint8_t>(1, 2) = 0x80;
		// Deep copy:
		CImage b = a.makeDeepCopy();
		EXPECT_EQ(b.at<uint8_t>(1, 2), 0x80);

		a.clear();
		a.resize(30, 30, CH_RGB);
		b.at<uint8_t>(1, 3) = 0x0;
		a.at<uint8_t>(1, 3) = 0x81;
		EXPECT_NE(b.at<uint8_t>(1, 3), 0x81);
	}

	{
		CImage a(20, 10, CH_GRAY);
		a.at<uint8_t>(1, 2) = 0x80;
		// move:
		CImage b = std::move(a);
		EXPECT_EQ(b.getWidth(), 20U);
		EXPECT_EQ(b.getHeight(), 10U);
		EXPECT_EQ(b.at<uint8_t>(1, 2), 0x80);
	}

	{
		CImage a(20, 10, CH_GRAY), b;
		a.at<uint8_t>(1, 2) = 0x80;
		// swap:
		a.swap(b);
		EXPECT_EQ(b.getWidth(), 20U);
		EXPECT_EQ(b.getHeight(), 10U);
		EXPECT_EQ(b.at<uint8_t>(1, 2), 0x80);
	}
}

TEST(CImage, ExternalImage)
{
	using namespace mrpt::img;
	{
		CImage a;
		a.setExternalStorage(tstImgFileColor);
		// Test automatic load-on-the-fly:
		EXPECT_EQ(a.getWidth(), 320U);
		EXPECT_EQ(a.getHeight(), 240U);
	}
}

#endif  // MRPT_HAS_OPENCV
