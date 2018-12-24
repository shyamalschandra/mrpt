/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          http://www.mrpt.org/                          |
   |                                                                        |
   | Copyright (c) 2005-2018, Individual contributors, see AUTHORS file     |
   | See: http://www.mrpt.org/Authors - All rights reserved.                |
   | Released under BSD License. See details in http://www.mrpt.org/License |
   +------------------------------------------------------------------------+ */

#include <mrpt/img/CImage.h>
#include <CTraitsTest.h>
#include <gtest/gtest.h>

template class mrpt::CTraitsTest<mrpt::img::CImage>;

TEST(CImage, CtorDefault)
{
	mrpt::img::CImage img;
	EXPECT_THROW(img.getWidth(), std::exception);
}

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
