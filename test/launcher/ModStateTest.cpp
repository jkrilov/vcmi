/*
 * ModStateTest.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "../../launcher/StdInc.h"

#include <gtest/gtest.h>

#include "../../launcher/modManager/modstate.h"
#include "../../lib/json/JsonNode.h"
#include "../../lib/modding/ModDescription.h"

TEST(ModStateTest, releaseDateBelongsOnlyToRepositoryVersion)
{
	JsonNode local;
	local["version"].String() = "3.0";
	JsonNode repository;
	repository["version"].String() = "2.0";
	repository["updateDate"].String() = "2024-12-30T12:00:00Z";
	ModDescription description("release-date-test", local, repository);
	ModState mod(description);

	EXPECT_EQ(mod.getReleaseDate("2.0"), QDateTime::fromString("2024-12-30T12:00:00Z", Qt::ISODate).toLocalTime());
	EXPECT_FALSE(mod.getReleaseDate("1.0").isValid());
	EXPECT_FALSE(mod.getReleaseDate("3.0").isValid());
	EXPECT_FALSE(mod.getReleaseDate({}).isValid());
	EXPECT_TRUE(mod.getUpdateDate().isEmpty());
}

TEST(ModStateTest, unavailableReleaseDatesRemainInvalid)
{
	for(const auto & date : {"", "not-a-date"})
	{
		JsonNode repository;
		repository["version"].String() = "2.0";
		if(*date)
			repository["updateDate"].String() = date;
		ModDescription description("release-date-test", JsonNode(), repository);
		ModState mod(description);

		EXPECT_FALSE(mod.getReleaseDate("2.0").isValid()) << date;
		EXPECT_TRUE(mod.getUpdateDate().isEmpty()) << date;
	}
}

TEST(ModStateTest, releaseDatePreservesInstantWithTimezoneOffset)
{
	JsonNode repository;
	repository["version"].String() = "2.0";
	repository["updateDate"].String() = "2025-01-01T00:30:00+02:00";
	ModDescription description("release-date-test", JsonNode(), repository);
	ModState mod(description);

	const auto date = mod.getReleaseDate("2.0");
	EXPECT_EQ(date.timeSpec(), Qt::LocalTime);
	EXPECT_EQ(date.toUTC(), QDateTime::fromString("2024-12-31T22:30:00Z", Qt::ISODate));
}
