/*
 * $Header: /fuji/lewis/RCS/NamedColors.cc,v 1.4 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: NamedColors.cc,v $
 *		Revision 1.4  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/21  20:22:12  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.8  1992/06/09  16:18:32  sterling
 *		reset Mapping size to speed things up
 *
 *		Revision 1.6  92/05/22  23:22:22  23:22:22  lewis (Lewis Pringle)
 *		Patch and hack named color database so it compiles on a mac. Also, we probably need a different
 *		representation. This is stagaringly slow/memory intensive on the mac - do some testing and verify,
 *		but my first try (debug=1) took almost a minute to build the Mapping, and made Emily run out of
 *		memory (had to beef up Finder partitiion).
 *		
 *		Revision 1.5  92/05/21  17:33:14  17:33:14  lewis (Lewis Pringle)
 *		React to change in definition of String::SubString().
 *		
 *		Revision 1.1  92/05/19  13:42:12  13:42:12  sterling (Sterling Wight)
 *		Initial revision
 *		
 *		
 *		
 *
 */

#include	"Format.hh"

#include	"NamedColors.hh"

#if		!qRealTemplatesAvailable
	Implement (Iterator, Color);
	Implement (Collection, Color);
	AbMappingImplement (String, Color);
	Mapping_HTImplement1 (String, Color);
	Mapping_HTImplement2 (String, Color);
	Mapping_HTImplement3 (String, Color);
#endif




// hack cuz apple compiler barfs - LGP May 22, 1992
static	void	Enter_D (AbMapping(String, Color)& d, const char* s, ColorComponent r, ColorComponent g, ColorComponent b)
{
	d.Enter (s, Color (r, g, b));
}

static	void	FillMapping1 (AbMapping(String, Color)& d)
{
	//
	// Output of ColorTableMunger program operating on some X11 rgb.txt file
	//
	Enter_D (d, "AliceBlue", 61680, 63736, 65535);
	Enter_D (d, "AntiqueWhite", 64250, 60395, 55255);
	Enter_D (d, "AntiqueWhite1", 65535, 61423, 56283);
	Enter_D (d, "AntiqueWhite2", 61166, 57311, 52428);
	Enter_D (d, "AntiqueWhite3", 52685, 49344, 45232);
	Enter_D (d, "AntiqueWhite4", 35723, 33667, 30840);
	Enter_D (d, "Aquamarine", 12850, 49087, 49601);
	Enter_D (d, "Black", 0, 0, 0);
	Enter_D (d, "BlanchedAlmond", 65535, 60395, 52685);
	Enter_D (d, "Blue", 0, 0, 65535);
	Enter_D (d, "BlueViolet", 35466, 11051, 58082);
	Enter_D (d, "Brown", 42405, 10794, 10794);
	Enter_D (d, "CadetBlue", 24415, 37522, 40606);
	Enter_D (d, "CadetBlue1", 39064, 62965, 65535);
	Enter_D (d, "CadetBlue2", 36494, 58853, 61166);
	Enter_D (d, "CadetBlue3", 31354, 50629, 52685);
	Enter_D (d, "CadetBlue4", 21331, 34438, 35723);
	Enter_D (d, "Coral", 65535, 29298, 22102);
	Enter_D (d, "CornflowerBlue", 8738, 8738, 39064);
	Enter_D (d, "Cyan", 0, 65535, 65535);
	Enter_D (d, "DarkGoldenrod", 47288, 34438, 2827);
	Enter_D (d, "DarkGoldenrod1", 65535, 47545, 3855);
	Enter_D (d, "DarkGoldenrod2", 61166, 44461, 3598);
	Enter_D (d, "DarkGoldenrod3", 52685, 38293, 3084);
	Enter_D (d, "DarkGoldenrod4", 35723, 25957, 2056);
	Enter_D (d, "DarkGreen", 0, 22102, 11565);
	Enter_D (d, "DarkKhaki", 48573, 47031, 27499);
	Enter_D (d, "DarkOliveGreen", 21845, 22102, 12079);
	Enter_D (d, "DarkOliveGreen1", 51914, 65535, 28784);
	Enter_D (d, "DarkOliveGreen2", 48316, 61166, 26728);
	Enter_D (d, "DarkOliveGreen3", 41634, 52685, 23130);
	Enter_D (d, "DarkOliveGreen4", 28270, 35723, 15677);
	Enter_D (d, "DarkOrange", 65535, 35980, 0);
	Enter_D (d, "DarkOrange1", 65535, 32639, 0);
	Enter_D (d, "DarkOrange2", 61166, 30326, 0);
	Enter_D (d, "DarkOrange3", 52685, 26214, 0);
	Enter_D (d, "DarkOrange4", 35723, 17733, 0);
	Enter_D (d, "DarkOrchid", 35723, 8224, 35723);
	Enter_D (d, "DarkOrchid1", 49087, 15934, 65535);
	Enter_D (d, "DarkOrchid2", 45746, 14906, 61166);
	Enter_D (d, "DarkOrchid3", 39578, 12850, 52685);
	Enter_D (d, "DarkOrchid4", 26728, 8738, 35723);
	Enter_D (d, "DarkSalmon", 59881, 38550, 31354);
	Enter_D (d, "DarkSeaGreen", 36751, 48316, 36751);
	Enter_D (d, "DarkSeaGreen1", 49601, 65535, 49601);
	Enter_D (d, "DarkSeaGreen2", 46260, 61166, 46260);
	Enter_D (d, "DarkSeaGreen3", 39835, 52685, 39835);
	Enter_D (d, "DarkSeaGreen4", 26985, 35723, 26985);
	Enter_D (d, "DarkSlateBlue", 14392, 19275, 26214);
	Enter_D (d, "DarkSlateGray", 12079, 20303, 20303);
	Enter_D (d, "DarkSlateGray1", 38807, 65535, 65535);
	Enter_D (d, "DarkSlateGray2", 36237, 61166, 61166);
	Enter_D (d, "DarkSlateGray3", 31097, 52685, 52685);
	Enter_D (d, "DarkSlateGray4", 21074, 35723, 35723);
	Enter_D (d, "DarkSlateGrey", 12079, 20303, 20303);
	Enter_D (d, "DarkTurquoise", 0, 42662, 42662);
	Enter_D (d, "DarkViolet", 38036, 0, 54227);
	Enter_D (d, "DeepPink", 65535, 5140, 37779);
	Enter_D (d, "DeepPink1", 65535, 5140, 37779);
	Enter_D (d, "DeepPink2", 61166, 4626, 35209);
	Enter_D (d, "DeepPink3", 52685, 4112, 30326);
	Enter_D (d, "DeepPink4", 35723, 2570, 20560);
	Enter_D (d, "DeepSkyBlue", 0, 49087, 65535);
	Enter_D (d, "DeepSkyBlue1", 0, 49087, 65535);
	Enter_D (d, "DeepSkyBlue2", 0, 45746, 61166);
	Enter_D (d, "DeepSkyBlue3", 0, 39578, 52685);
	Enter_D (d, "DeepSkyBlue4", 0, 26728, 35723);
	Enter_D (d, "DimGray", 21588, 21588, 21588);
	Enter_D (d, "DimGrey", 21588, 21588, 21588);
	Enter_D (d, "DodgerBlue", 7710, 37008, 65535);
	Enter_D (d, "DodgerBlue1", 7710, 37008, 65535);
	Enter_D (d, "DodgerBlue2", 7196, 34438, 61166);
	Enter_D (d, "DodgerBlue3", 6168, 29812, 52685);
	Enter_D (d, "DodgerBlue4", 4112, 20046, 35723);
	Enter_D (d, "Firebrick", 36494, 8995, 8995);
	Enter_D (d, "FloralWhite", 65535, 64250, 61680);
	Enter_D (d, "ForestGreen", 20560, 40863, 26985);
	Enter_D (d, "GhostWhite", 63736, 63736, 65535);
	Enter_D (d, "Gold", 56026, 43690, 0);
	Enter_D (d, "Goldenrod", 61423, 57311, 33924);
	Enter_D (d, "Gray", 32382, 32382, 32382);
	Enter_D (d, "Gray0", 0, 0, 0);
	Enter_D (d, "Gray1", 771, 771, 771);
	Enter_D (d, "Gray10", 6682, 6682, 6682);
	Enter_D (d, "Gray100", 65535, 65535, 65535);
	Enter_D (d, "Gray11", 7196, 7196, 7196);
	Enter_D (d, "Gray12", 7967, 7967, 7967);
	Enter_D (d, "Gray13", 8481, 8481, 8481);
	Enter_D (d, "Gray14", 9252, 9252, 9252);
	Enter_D (d, "Gray15", 9766, 9766, 9766);
	Enter_D (d, "Gray16", 10537, 10537, 10537);
	Enter_D (d, "Gray17", 11051, 11051, 11051);
	Enter_D (d, "Gray18", 11822, 11822, 11822);
	Enter_D (d, "Gray19", 12336, 12336, 12336);
	Enter_D (d, "Gray2", 1285, 1285, 1285);
	Enter_D (d, "Gray20", 13107, 13107, 13107);
	Enter_D (d, "Gray21", 13878, 13878, 13878);
	Enter_D (d, "Gray22", 14392, 14392, 14392);
	Enter_D (d, "Gray23", 15163, 15163, 15163);
	Enter_D (d, "Gray24", 15677, 15677, 15677);
	Enter_D (d, "Gray25", 16448, 16448, 16448);
	Enter_D (d, "Gray26", 16962, 16962, 16962);
	Enter_D (d, "Gray27", 17733, 17733, 17733);
	Enter_D (d, "Gray28", 18247, 18247, 18247);
	Enter_D (d, "Gray29", 19018, 19018, 19018);
	Enter_D (d, "Gray3", 2056, 2056, 2056);
	Enter_D (d, "Gray30", 19789, 19789, 19789);
	Enter_D (d, "Gray31", 20303, 20303, 20303);
	Enter_D (d, "Gray32", 21074, 21074, 21074);
	Enter_D (d, "Gray33", 21588, 21588, 21588);
	Enter_D (d, "Gray34", 22359, 22359, 22359);
	Enter_D (d, "Gray35", 22873, 22873, 22873);
	Enter_D (d, "Gray36", 23644, 23644, 23644);
	Enter_D (d, "Gray37", 24158, 24158, 24158);
	Enter_D (d, "Gray38", 24929, 24929, 24929);
	Enter_D (d, "Gray39", 25443, 25443, 25443);
	Enter_D (d, "Gray4", 2570, 2570, 2570);
	Enter_D (d, "Gray40", 26214, 26214, 26214);
	Enter_D (d, "Gray41", 26985, 26985, 26985);
	Enter_D (d, "Gray42", 27499, 27499, 27499);
	Enter_D (d, "Gray43", 28270, 28270, 28270);
	Enter_D (d, "Gray44", 28784, 28784, 28784);
	Enter_D (d, "Gray45", 29555, 29555, 29555);
	Enter_D (d, "Gray46", 30069, 30069, 30069);
	Enter_D (d, "Gray47", 30840, 30840, 30840);
	Enter_D (d, "Gray48", 31354, 31354, 31354);
	Enter_D (d, "Gray49", 32125, 32125, 32125);
	Enter_D (d, "Gray5", 3341, 3341, 3341);
	Enter_D (d, "Gray50", 32639, 32639, 32639);
	Enter_D (d, "Gray51", 33410, 33410, 33410);
	Enter_D (d, "Gray52", 34181, 34181, 34181);
	Enter_D (d, "Gray53", 34695, 34695, 34695);
	Enter_D (d, "Gray54", 35466, 35466, 35466);
	Enter_D (d, "Gray55", 35980, 35980, 35980);
	Enter_D (d, "Gray56", 36751, 36751, 36751);
	Enter_D (d, "Gray57", 37265, 37265, 37265);
	Enter_D (d, "Gray58", 38036, 38036, 38036);
	Enter_D (d, "Gray59", 38550, 38550, 38550);
	Enter_D (d, "Gray6", 3855, 3855, 3855);
	Enter_D (d, "Gray60", 39321, 39321, 39321);
	Enter_D (d, "Gray61", 40092, 40092, 40092);
	Enter_D (d, "Gray62", 40606, 40606, 40606);
	Enter_D (d, "Gray63", 41377, 41377, 41377);
	Enter_D (d, "Gray64", 41891, 41891, 41891);
	Enter_D (d, "Gray65", 42662, 42662, 42662);
	Enter_D (d, "Gray66", 43176, 43176, 43176);
	Enter_D (d, "Gray67", 43947, 43947, 43947);
	Enter_D (d, "Gray68", 44461, 44461, 44461);
	Enter_D (d, "Gray69", 45232, 45232, 45232);
	Enter_D (d, "Gray7", 4626, 4626, 4626);
	Enter_D (d, "Gray70", 46003, 46003, 46003);
	Enter_D (d, "Gray71", 46517, 46517, 46517);
	Enter_D (d, "Gray72", 47288, 47288, 47288);
	Enter_D (d, "Gray73", 47802, 47802, 47802);
	Enter_D (d, "Gray74", 48573, 48573, 48573);
	Enter_D (d, "Gray75", 49087, 49087, 49087);
	Enter_D (d, "Gray76", 49858, 49858, 49858);
	Enter_D (d, "Gray77", 50372, 50372, 50372);
	Enter_D (d, "Gray78", 51143, 51143, 51143);
	Enter_D (d, "Gray79", 51657, 51657, 51657);
	Enter_D (d, "Gray8", 5140, 5140, 5140);
	Enter_D (d, "Gray80", 52428, 52428, 52428);
	Enter_D (d, "Gray81", 53199, 53199, 53199);
	Enter_D (d, "Gray82", 53713, 53713, 53713);
	Enter_D (d, "Gray83", 54484, 54484, 54484);
	Enter_D (d, "Gray84", 54998, 54998, 54998);
	Enter_D (d, "Gray85", 55769, 55769, 55769);
	Enter_D (d, "Gray86", 56283, 56283, 56283);
	Enter_D (d, "Gray87", 57054, 57054, 57054);
	Enter_D (d, "Gray88", 57568, 57568, 57568);
	Enter_D (d, "Gray89", 58339, 58339, 58339);
	Enter_D (d, "Gray9", 5911, 5911, 5911);
	Enter_D (d, "Gray90", 58853, 58853, 58853);
	Enter_D (d, "Gray91", 59624, 59624, 59624);
	Enter_D (d, "Gray92", 60395, 60395, 60395);
	Enter_D (d, "Gray93", 60909, 60909, 60909);
	Enter_D (d, "Gray94", 61680, 61680, 61680);
	Enter_D (d, "Gray95", 62194, 62194, 62194);
	Enter_D (d, "Gray96", 62965, 62965, 62965);
	Enter_D (d, "Gray97", 63479, 63479, 63479);
	Enter_D (d, "Gray98", 64250, 64250, 64250);
	Enter_D (d, "Gray99", 64764, 64764, 64764);
	Enter_D (d, "Green", 0, 65535, 0);
	Enter_D (d, "GreenYellow", 44461, 65535, 12079);
	Enter_D (d, "Grey", 32382, 32382, 32382);
	Enter_D (d, "Grey0", 0, 0, 0);
	Enter_D (d, "Grey1", 771, 771, 771);
	Enter_D (d, "Grey10", 6682, 6682, 6682);
	Enter_D (d, "Grey100", 65535, 65535, 65535);
	Enter_D (d, "Grey11", 7196, 7196, 7196);
	Enter_D (d, "Grey12", 7967, 7967, 7967);
	Enter_D (d, "Grey13", 8481, 8481, 8481);
	Enter_D (d, "Grey14", 9252, 9252, 9252);
	Enter_D (d, "Grey15", 9766, 9766, 9766);
	Enter_D (d, "Grey16", 10537, 10537, 10537);
	Enter_D (d, "Grey17", 11051, 11051, 11051);
	Enter_D (d, "Grey18", 11822, 11822, 11822);
	Enter_D (d, "Grey19", 12336, 12336, 12336);
	Enter_D (d, "Grey2", 1285, 1285, 1285);
	Enter_D (d, "Grey20", 13107, 13107, 13107);
	Enter_D (d, "Grey21", 13878, 13878, 13878);
	Enter_D (d, "Grey22", 14392, 14392, 14392);
	Enter_D (d, "Grey23", 15163, 15163, 15163);
	Enter_D (d, "Grey24", 15677, 15677, 15677);
	Enter_D (d, "Grey25", 16448, 16448, 16448);
	Enter_D (d, "Grey26", 16962, 16962, 16962);
	Enter_D (d, "Grey27", 17733, 17733, 17733);
	Enter_D (d, "Grey28", 18247, 18247, 18247);
	Enter_D (d, "Grey29", 19018, 19018, 19018);
	Enter_D (d, "Grey3", 2056, 2056, 2056);
	Enter_D (d, "Grey30", 19789, 19789, 19789);
	Enter_D (d, "Grey31", 20303, 20303, 20303);
	Enter_D (d, "Grey32", 21074, 21074, 21074);
	Enter_D (d, "Grey33", 21588, 21588, 21588);
	Enter_D (d, "Grey34", 22359, 22359, 22359);
	Enter_D (d, "Grey35", 22873, 22873, 22873);
	Enter_D (d, "Grey36", 23644, 23644, 23644);
	Enter_D (d, "Grey37", 24158, 24158, 24158);
	Enter_D (d, "Grey38", 24929, 24929, 24929);
	Enter_D (d, "Grey39", 25443, 25443, 25443);
	Enter_D (d, "Grey4", 2570, 2570, 2570);
	Enter_D (d, "Grey40", 26214, 26214, 26214);
	Enter_D (d, "Grey41", 26985, 26985, 26985);
	Enter_D (d, "Grey42", 27499, 27499, 27499);
	Enter_D (d, "Grey43", 28270, 28270, 28270);
	Enter_D (d, "Grey44", 28784, 28784, 28784);
	Enter_D (d, "Grey45", 29555, 29555, 29555);
	Enter_D (d, "Grey46", 30069, 30069, 30069);
	Enter_D (d, "Grey47", 30840, 30840, 30840);
	Enter_D (d, "Grey48", 31354, 31354, 31354);
	Enter_D (d, "Grey49", 32125, 32125, 32125);
	Enter_D (d, "Grey5", 3341, 3341, 3341);
	Enter_D (d, "Grey50", 32639, 32639, 32639);
	Enter_D (d, "Grey51", 33410, 33410, 33410);
	Enter_D (d, "Grey52", 34181, 34181, 34181);
	Enter_D (d, "Grey53", 34695, 34695, 34695);
	Enter_D (d, "Grey54", 35466, 35466, 35466);
	Enter_D (d, "Grey55", 35980, 35980, 35980);
	Enter_D (d, "Grey56", 36751, 36751, 36751);
	Enter_D (d, "Grey57", 37265, 37265, 37265);
	Enter_D (d, "Grey58", 38036, 38036, 38036);
	Enter_D (d, "Grey59", 38550, 38550, 38550);
	Enter_D (d, "Grey6", 3855, 3855, 3855);
	Enter_D (d, "Grey60", 39321, 39321, 39321);
	Enter_D (d, "Grey61", 40092, 40092, 40092);
	Enter_D (d, "Grey62", 40606, 40606, 40606);
	Enter_D (d, "Grey63", 41377, 41377, 41377);
	Enter_D (d, "Grey64", 41891, 41891, 41891);
	Enter_D (d, "Grey65", 42662, 42662, 42662);
	Enter_D (d, "Grey66", 43176, 43176, 43176);
	Enter_D (d, "Grey67", 43947, 43947, 43947);
	Enter_D (d, "Grey68", 44461, 44461, 44461);
	Enter_D (d, "Grey69", 45232, 45232, 45232);
	Enter_D (d, "Grey7", 4626, 4626, 4626);
	Enter_D (d, "Grey70", 46003, 46003, 46003);
	Enter_D (d, "Grey71", 46517, 46517, 46517);
	Enter_D (d, "Grey72", 47288, 47288, 47288);
	Enter_D (d, "Grey73", 47802, 47802, 47802);
	Enter_D (d, "Grey74", 48573, 48573, 48573);
	Enter_D (d, "Grey75", 49087, 49087, 49087);
	Enter_D (d, "Grey76", 49858, 49858, 49858);
	Enter_D (d, "Grey77", 50372, 50372, 50372);
	Enter_D (d, "Grey78", 51143, 51143, 51143);
	Enter_D (d, "Grey79", 51657, 51657, 51657);
	Enter_D (d, "Grey8", 5140, 5140, 5140);
	Enter_D (d, "Grey80", 52428, 52428, 52428);
	Enter_D (d, "Grey81", 53199, 53199, 53199);
	Enter_D (d, "Grey82", 53713, 53713, 53713);
	Enter_D (d, "Grey83", 54484, 54484, 54484);
	Enter_D (d, "Grey84", 54998, 54998, 54998);
	Enter_D (d, "Grey85", 55769, 55769, 55769);
	Enter_D (d, "Grey86", 56283, 56283, 56283);
	Enter_D (d, "Grey87", 57054, 57054, 57054);
	Enter_D (d, "Grey88", 57568, 57568, 57568);
	Enter_D (d, "Grey89", 58339, 58339, 58339);
	Enter_D (d, "Grey9", 5911, 5911, 5911);
	Enter_D (d, "Grey90", 58853, 58853, 58853);
	Enter_D (d, "Grey91", 59624, 59624, 59624);
	Enter_D (d, "Grey92", 60395, 60395, 60395);
	Enter_D (d, "Grey93", 60909, 60909, 60909);
	Enter_D (d, "Grey94", 61680, 61680, 61680);
	Enter_D (d, "Grey95", 62194, 62194, 62194);
	Enter_D (d, "Grey96", 62965, 62965, 62965);
	Enter_D (d, "Grey97", 63479, 63479, 63479);
	Enter_D (d, "Grey98", 64250, 64250, 64250);
	Enter_D (d, "Grey99", 64764, 64764, 64764);

// Continued in next function cuz of MPW compiler (C) bugs - blows  limits...
}

static	void	FillMapping2 (AbMapping(String, Color)& d)
{
	Enter_D (d, "HotPink", 65535, 26985, 46260);
	Enter_D (d, "HotPink1", 65535, 28270, 46260);
	Enter_D (d, "HotPink2", 61166, 27242, 42919);
	Enter_D (d, "HotPink3", 52685, 24672, 37008);
	Enter_D (d, "HotPink4", 35723, 14906, 25186);
	Enter_D (d, "IndianRed", 27499, 14649, 14649);
	Enter_D (d, "IndianRed1", 65535, 27242, 27242);
	Enter_D (d, "IndianRed2", 61166, 25443, 25443);
	Enter_D (d, "IndianRed3", 52685, 21845, 21845);
	Enter_D (d, "IndianRed4", 35723, 14906, 14906);
	Enter_D (d, "Khaki", 46003, 46003, 32382);
	Enter_D (d, "LavenderBlush", 65535, 61680, 62965);
	Enter_D (d, "LavenderBlush1", 65535, 61680, 62965);
	Enter_D (d, "LavenderBlush2", 61166, 57568, 58853);
	Enter_D (d, "LavenderBlush3", 52685, 49601, 50629);
	Enter_D (d, "LavenderBlush4", 35723, 33667, 34438);
	Enter_D (d, "LawnGreen", 31868, 64764, 0);
	Enter_D (d, "LemonChiffon", 65535, 64250, 52685);
	Enter_D (d, "LemonChiffon1", 65535, 64250, 52685);
	Enter_D (d, "LemonChiffon2", 61166, 59881, 49087);
	Enter_D (d, "LemonChiffon3", 52685, 51657, 42405);
	Enter_D (d, "LemonChiffon4", 35723, 35209, 28784);
	Enter_D (d, "LightBlue", 45232, 58082, 65535);
	Enter_D (d, "LightBlue1", 49087, 61423, 65535);
	Enter_D (d, "LightBlue2", 45746, 57311, 61166);
	Enter_D (d, "LightBlue3", 39578, 49344, 52685);
	Enter_D (d, "LightBlue4", 26728, 33667, 35723);
	Enter_D (d, "LightCoral", 61680, 32896, 32896);
	Enter_D (d, "LightCyan", 57568, 65535, 65535);
	Enter_D (d, "LightCyan1", 57568, 65535, 65535);
	Enter_D (d, "LightCyan2", 53713, 61166, 61166);
	Enter_D (d, "LightCyan3", 46260, 52685, 52685);
	Enter_D (d, "LightCyan4", 31354, 35723, 35723);
	Enter_D (d, "LightGoldenrod", 61166, 56797, 33410);
	Enter_D (d, "LightGoldenrod1", 65535, 60652, 35723);
	Enter_D (d, "LightGoldenrod2", 61166, 56540, 33410);
	Enter_D (d, "LightGoldenrod3", 52685, 48830, 28784);
	Enter_D (d, "LightGoldenrod4", 35723, 33153, 19532);
	Enter_D (d, "LightGoldenrodYellow", 64250, 64250, 53970);
	Enter_D (d, "LightGray", 43176, 43176, 43176);
	Enter_D (d, "LightGrey", 43176, 43176, 43176);
	Enter_D (d, "LightPink", 65535, 46774, 49601);
	Enter_D (d, "LightPink1", 65535, 44718, 47545);
	Enter_D (d, "LightPink2", 61166, 41634, 44461);
	Enter_D (d, "LightPink3", 52685, 35980, 38293);
	Enter_D (d, "LightPink4", 35723, 24415, 25957);
	Enter_D (d, "LightSalmon", 65535, 41120, 31354);
	Enter_D (d, "LightSalmon1", 65535, 41120, 31354);
	Enter_D (d, "LightSalmon2", 61166, 38293, 29298);
	Enter_D (d, "LightSalmon3", 52685, 33153, 25186);
	Enter_D (d, "LightSalmon4", 35723, 22359, 16962);
	Enter_D (d, "LightSeaGreen", 8224, 45746, 43690);
	Enter_D (d, "LightSkyBlue", 34695, 52942, 64250);
	Enter_D (d, "LightSkyBlue1", 45232, 58082, 65535);
	Enter_D (d, "LightSkyBlue2", 42148, 54227, 61166);
	Enter_D (d, "LightSkyBlue3", 36237, 46774, 52685);
	Enter_D (d, "LightSkyBlue4", 24672, 31611, 35723);
	Enter_D (d, "LightSlateBlue", 33924, 28784, 65535);
	Enter_D (d, "LightSlateGray", 30583, 34952, 39321);
	Enter_D (d, "LightSlateGrey", 30583, 34952, 39321);
	Enter_D (d, "LightSteelBlue", 31868, 39064, 54227);
	Enter_D (d, "LightSteelBlue1", 51914, 57825, 65535);
	Enter_D (d, "LightSteelBlue2", 48316, 53970, 61166);
	Enter_D (d, "LightSteelBlue3", 41634, 46517, 52685);
	Enter_D (d, "LightSteelBlue4", 28270, 31611, 35723);
	Enter_D (d, "LightYellow", 65535, 65535, 57568);
	Enter_D (d, "LightYellow1", 65535, 65535, 57568);
	Enter_D (d, "LightYellow2", 61166, 61166, 53713);
	Enter_D (d, "LightYellow3", 52685, 52685, 46260);
	Enter_D (d, "LightYellow4", 35723, 35723, 31354);
	Enter_D (d, "LimeGreen", 0, 44975, 5140);
	Enter_D (d, "Magenta", 65535, 0, 65535);
	Enter_D (d, "Maroon", 36751, 0, 21074);
	Enter_D (d, "MediumAquamarine", 0, 37779, 36751);
	Enter_D (d, "MediumBlue", 12850, 12850, 52428);
	Enter_D (d, "MediumForestGreen", 12850, 33153, 19275);
	Enter_D (d, "MediumGoldenrod", 53713, 49601, 26214);
	Enter_D (d, "MediumOrchid", 48573, 21074, 48573);
	Enter_D (d, "MediumOrchid1", 57568, 26214, 65535);
	Enter_D (d, "MediumOrchid2", 53713, 24415, 61166);
	Enter_D (d, "MediumOrchid3", 46260, 21074, 52685);
	Enter_D (d, "MediumOrchid4", 31354, 14135, 35723);
	Enter_D (d, "MediumPurple", 37779, 28784, 56283);
	Enter_D (d, "MediumPurple1", 43947, 33410, 65535);
	Enter_D (d, "MediumPurple2", 40863, 31097, 61166);
	Enter_D (d, "MediumPurple3", 35209, 26728, 52685);
	Enter_D (d, "MediumPurple4", 23901, 18247, 35723);
	Enter_D (d, "MediumSeaGreen", 13364, 30583, 26214);
	Enter_D (d, "MediumSlateBlue", 27242, 27242, 36237);
	Enter_D (d, "MediumSpringGreen", 8995, 36494, 8995);
	Enter_D (d, "MediumTurquoise", 0, 53970, 53970);
	Enter_D (d, "MediumVioletRed", 54741, 8224, 31097);
	Enter_D (d, "MidnightBlue", 12079, 12079, 25700);
	Enter_D (d, "MintCream", 62965, 65535, 64250);
	Enter_D (d, "MistyRose", 65535, 58596, 57825);
	Enter_D (d, "MistyRose1", 65535, 58596, 57825);
	Enter_D (d, "MistyRose2", 61166, 54741, 53970);
	Enter_D (d, "MistyRose3", 52685, 47031, 46517);
	Enter_D (d, "MistyRose4", 35723, 32125, 31611);
	Enter_D (d, "NavajoWhite", 65535, 57054, 44461);
	Enter_D (d, "NavajoWhite1", 65535, 57054, 44461);
	Enter_D (d, "NavajoWhite2", 61166, 53199, 41377);
	Enter_D (d, "NavajoWhite3", 52685, 46003, 35723);
	Enter_D (d, "NavajoWhite4", 35723, 31097, 24158);
	Enter_D (d, "Navy", 8995, 8995, 30069);
	Enter_D (d, "NavyBlue", 8995, 8995, 30069);
	Enter_D (d, "OldLace", 65021, 62965, 59110);
	Enter_D (d, "OliveDrab", 27499, 36494, 8995);
	Enter_D (d, "OliveDrab1", 49344, 65535, 15934);
	Enter_D (d, "OliveDrab2", 46003, 61166, 14906);
	Enter_D (d, "OliveDrab3", 39578, 52685, 12850);
	Enter_D (d, "OliveDrab4", 26985, 35723, 8738);
	Enter_D (d, "Orange", 65535, 34695, 0);
	Enter_D (d, "OrangeRed", 65535, 17733, 0);
	Enter_D (d, "OrangeRed1", 65535, 17733, 0);
	Enter_D (d, "OrangeRed2", 61166, 16448, 0);
	Enter_D (d, "OrangeRed3", 52685, 14135, 0);
	Enter_D (d, "OrangeRed4", 35723, 9509, 0);
	Enter_D (d, "Orchid", 61423, 33924, 61423);
	Enter_D (d, "PaleGoldenrod", 61166, 59624, 43690);
	Enter_D (d, "PaleGreen", 29555, 57054, 30840);
	Enter_D (d, "PaleGreen1", 39578, 65535, 39578);
	Enter_D (d, "PaleGreen2", 37008, 61166, 37008);
	Enter_D (d, "PaleGreen3", 31868, 52685, 31868);
	Enter_D (d, "PaleGreen4", 21588, 35723, 21588);
	Enter_D (d, "PaleTurquoise", 44975, 61166, 61166);
	Enter_D (d, "PaleTurquoise1", 48059, 65535, 65535);
	Enter_D (d, "PaleTurquoise2", 44718, 61166, 61166);
	Enter_D (d, "PaleTurquoise3", 38550, 52685, 52685);
	Enter_D (d, "PaleTurquoise4", 26214, 35723, 35723);
	Enter_D (d, "PaleVioletRed", 56283, 28784, 37779);
	Enter_D (d, "PaleVioletRed1", 65535, 33410, 43947);
	Enter_D (d, "PaleVioletRed2", 61166, 31097, 40863);
	Enter_D (d, "PaleVioletRed3", 52685, 26728, 35209);
	Enter_D (d, "PaleVioletRed4", 35723, 18247, 23901);
	Enter_D (d, "PapayaWhip", 65535, 61423, 54741);
	Enter_D (d, "PeachPuff", 65535, 56026, 47545);
	Enter_D (d, "PeachPuff1", 65535, 56026, 47545);
	Enter_D (d, "PeachPuff2", 61166, 52171, 44461);
	Enter_D (d, "PeachPuff3", 52685, 44975, 38293);
	Enter_D (d, "PeachPuff4", 35723, 30583, 25957);
	Enter_D (d, "Pink", 65535, 46517, 50629);
	Enter_D (d, "Plum", 50629, 18504, 39835);
	Enter_D (d, "PowderBlue", 45232, 57568, 59110);
	Enter_D (d, "Red", 65535, 0, 0);
	Enter_D (d, "RosyBrown", 48316, 36751, 36751);
	Enter_D (d, "RosyBrown1", 65535, 49601, 49601);
	Enter_D (d, "RosyBrown2", 61166, 46260, 46260);
	Enter_D (d, "RosyBrown3", 52685, 39835, 39835);
	Enter_D (d, "RosyBrown4", 35723, 26985, 26985);
	Enter_D (d, "RoyalBlue", 16705, 26985, 57825);
	Enter_D (d, "RoyalBlue1", 18504, 30326, 65535);
	Enter_D (d, "RoyalBlue2", 17219, 28270, 61166);
	Enter_D (d, "RoyalBlue3", 14906, 24415, 52685);
	Enter_D (d, "RoyalBlue4", 10023, 16448, 35723);
	Enter_D (d, "SaddleBrown", 35723, 17733, 4883);
	Enter_D (d, "Salmon", 59881, 38550, 31354);
	Enter_D (d, "SandyBrown", 62708, 42148, 24672);
	Enter_D (d, "SeaGreen", 21074, 38293, 33924);
	Enter_D (d, "SeaGreen1", 21588, 65535, 40863);
	Enter_D (d, "SeaGreen2", 20046, 61166, 38036);
	Enter_D (d, "SeaGreen3", 17219, 52685, 32896);
	Enter_D (d, "SeaGreen4", 11822, 35723, 22359);
	Enter_D (d, "Sienna", 38550, 21074, 11565);
	Enter_D (d, "SkyBlue", 29298, 40863, 65535);
	Enter_D (d, "SkyBlue1", 34695, 52942, 65535);
	Enter_D (d, "SkyBlue2", 32382, 49344, 61166);
	Enter_D (d, "SkyBlue3", 27756, 42662, 52685);
	Enter_D (d, "SkyBlue4", 19018, 28784, 35723);
	Enter_D (d, "SlateBlue", 32382, 34952, 43947);
	Enter_D (d, "SlateBlue1", 33667, 28527, 65535);
	Enter_D (d, "SlateBlue2", 31354, 26471, 61166);
	Enter_D (d, "SlateBlue3", 26985, 22873, 52685);
	Enter_D (d, "SlateBlue4", 18247, 15420, 35723);
	Enter_D (d, "SlateGray", 28784, 32896, 37008);
	Enter_D (d, "SlateGray1", 50886, 58082, 65535);
	Enter_D (d, "SlateGray2", 47545, 54227, 61166);
	Enter_D (d, "SlateGray3", 40863, 46774, 52685);
	Enter_D (d, "SlateGray4", 27756, 31611, 35723);
	Enter_D (d, "SlateGrey", 28784, 32896, 37008);
	Enter_D (d, "SpringGreen", 16705, 44204, 16705);
	Enter_D (d, "SpringGreen1", 0, 65535, 32639);
	Enter_D (d, "SpringGreen2", 0, 61166, 30326);
	Enter_D (d, "SpringGreen3", 0, 52685, 26214);
	Enter_D (d, "SpringGreen4", 0, 35723, 17733);
	Enter_D (d, "SteelBlue", 21588, 28784, 43690);
	Enter_D (d, "SteelBlue1", 25443, 47288, 65535);
	Enter_D (d, "SteelBlue2", 23644, 44204, 61166);
	Enter_D (d, "SteelBlue3", 20303, 38036, 52685);
	Enter_D (d, "SteelBlue4", 13878, 25700, 35723);
	Enter_D (d, "Tan", 57054, 47288, 34695);
	Enter_D (d, "Thistle", 55512, 49087, 55512);
	Enter_D (d, "Transparent", 0, 0, 257);
	Enter_D (d, "Turquoise", 6425, 52428, 57311);
	Enter_D (d, "Violet", 40092, 15934, 52942);
	Enter_D (d, "VioletRed", 62451, 15934, 38550);
	Enter_D (d, "VioletRed1", 65535, 15934, 38550);
	Enter_D (d, "VioletRed2", 61166, 14906, 35980);
	Enter_D (d, "VioletRed3", 52685, 12850, 30840);
	Enter_D (d, "VioletRed4", 35723, 8738, 21074);
	Enter_D (d, "Wheat", 62965, 57054, 46003);
	Enter_D (d, "White", 65535, 65535, 65535);
	Enter_D (d, "WhiteSmoke", 62965, 62965, 62965);
	Enter_D (d, "Yellow", 65535, 65535, 0);
	Enter_D (d, "YellowGreen", 12850, 55512, 14392);
	Enter_D (d, "alice blue", 61680, 63736, 65535);
	Enter_D (d, "antique white", 64250, 60395, 55255);
	Enter_D (d, "aquamarine", 12850, 49087, 49601);
	Enter_D (d, "aquamarine1", 32639, 65535, 54484);
	Enter_D (d, "aquamarine2", 30326, 61166, 50886);
	Enter_D (d, "aquamarine3", 26214, 52685, 43690);
	Enter_D (d, "aquamarine4", 17733, 35723, 29812);
	Enter_D (d, "azure", 61680, 65535, 65535);
	Enter_D (d, "azure1", 61680, 65535, 65535);
	Enter_D (d, "azure2", 57568, 61166, 61166);
	Enter_D (d, "azure3", 49601, 52685, 52685);
	Enter_D (d, "azure4", 33667, 35723, 35723);
	Enter_D (d, "beige", 62965, 62965, 56540);
	Enter_D (d, "bisque", 65535, 58596, 50372);
	Enter_D (d, "bisque1", 65535, 58596, 50372);
	Enter_D (d, "bisque2", 61166, 54741, 47031);
	Enter_D (d, "bisque3", 52685, 47031, 40606);
	Enter_D (d, "bisque4", 35723, 32125, 27499);
	Enter_D (d, "black", 0, 0, 0);
	Enter_D (d, "blanched almond", 65535, 60395, 52685);
	Enter_D (d, "blue", 0, 0, 65535);
	Enter_D (d, "blue1", 0, 0, 65535);
	Enter_D (d, "blue2", 0, 0, 61166);
	Enter_D (d, "blue3", 0, 0, 52685);
	Enter_D (d, "blue4", 0, 0, 35723);
	Enter_D (d, "blue violet", 35466, 11051, 58082);
	Enter_D (d, "brown", 42405, 10794, 10794);
	Enter_D (d, "brown1", 65535, 16448, 16448);
	Enter_D (d, "brown2", 61166, 15163, 15163);
	Enter_D (d, "brown3", 52685, 13107, 13107);
	Enter_D (d, "brown4", 35723, 8995, 8995);
	Enter_D (d, "burlywood", 57054, 47288, 34695);
	Enter_D (d, "burlywood1", 65535, 54227, 39835);
	Enter_D (d, "burlywood2", 61166, 50629, 37265);
	Enter_D (d, "burlywood3", 52685, 43690, 32125);
	Enter_D (d, "burlywood4", 35723, 29555, 21845);
	Enter_D (d, "cadet blue", 24415, 37522, 40606);
	Enter_D (d, "chartreuse", 32639, 65535, 0);
	Enter_D (d, "chartreuse1", 32639, 65535, 0);
	Enter_D (d, "chartreuse2", 30326, 61166, 0);
	Enter_D (d, "chartreuse3", 26214, 52685, 0);
	Enter_D (d, "chartreuse4", 17733, 35723, 0);
	Enter_D (d, "chocolate", 53970, 26985, 7710);
	Enter_D (d, "chocolate1", 65535, 32639, 9252);
	Enter_D (d, "chocolate2", 61166, 30326, 8481);
	Enter_D (d, "chocolate3", 52685, 26214, 7453);
	Enter_D (d, "chocolate4", 35723, 17733, 4883);
	Enter_D (d, "coral", 65535, 29298, 22102);
	Enter_D (d, "coral1", 65535, 29298, 22102);
	Enter_D (d, "coral2", 61166, 27242, 20560);
	Enter_D (d, "coral3", 52685, 23387, 17733);
	Enter_D (d, "coral4", 35723, 15934, 12079);
	Enter_D (d, "cornflower blue", 8738, 8738, 39064);
	Enter_D (d, "cornsilk", 65535, 63736, 56540);
	Enter_D (d, "cornsilk1", 65535, 63736, 56540);
	Enter_D (d, "cornsilk2", 61166, 59624, 52685);
	Enter_D (d, "cornsilk3", 52685, 51400, 45489);
	Enter_D (d, "cornsilk4", 35723, 34952, 30840);
	Enter_D (d, "cyan", 0, 65535, 65535);
	Enter_D (d, "cyan1", 0, 65535, 65535);
	Enter_D (d, "cyan2", 0, 61166, 61166);
	Enter_D (d, "cyan3", 0, 52685, 52685);
	Enter_D (d, "cyan4", 0, 35723, 35723);
	Enter_D (d, "dark goldenrod", 47288, 34438, 2827);
	Enter_D (d, "dark green", 0, 22102, 11565);
	Enter_D (d, "dark khaki", 48573, 47031, 27499);
	Enter_D (d, "dark olive green", 21845, 22102, 12079);
	Enter_D (d, "dark orange", 65535, 35980, 0);
	Enter_D (d, "dark orchid", 35723, 8224, 35723);
	Enter_D (d, "dark salmon", 59881, 38550, 31354);
	Enter_D (d, "dark sea green", 36751, 48316, 36751);
	Enter_D (d, "dark slate blue", 14392, 19275, 26214);
	Enter_D (d, "dark slate gray", 12079, 20303, 20303);
	Enter_D (d, "dark slate grey", 12079, 20303, 20303);
	Enter_D (d, "dark turquoise", 0, 42662, 42662);
	Enter_D (d, "dark violet", 38036, 0, 54227);
	Enter_D (d, "deep pink", 65535, 5140, 37779);
	Enter_D (d, "deep sky blue", 0, 49087, 65535);
	Enter_D (d, "dim gray", 21588, 21588, 21588);
	Enter_D (d, "dim grey", 21588, 21588, 21588);
	Enter_D (d, "dodger blue", 7710, 37008, 65535);
	Enter_D (d, "firebrick", 36494, 8995, 8995);
	Enter_D (d, "firebrick1", 65535, 12336, 12336);
	Enter_D (d, "firebrick2", 61166, 11308, 11308);
	Enter_D (d, "firebrick3", 52685, 9766, 9766);
	Enter_D (d, "firebrick4", 35723, 6682, 6682);
	Enter_D (d, "floral white", 65535, 64250, 61680);
	Enter_D (d, "forest green", 20560, 40863, 26985);
	Enter_D (d, "gainsboro", 56540, 56540, 56540);
	Enter_D (d, "ghost white", 63736, 63736, 65535);
	Enter_D (d, "gold", 56026, 43690, 0);
	Enter_D (d, "gold1", 65535, 55255, 0);
	Enter_D (d, "gold2", 61166, 51657, 0);
	Enter_D (d, "gold3", 52685, 44461, 0);
	Enter_D (d, "gold4", 35723, 30069, 0);
	Enter_D (d, "goldenrod", 61423, 57311, 33924);
	Enter_D (d, "goldenrod1", 65535, 49601, 9509);
	Enter_D (d, "goldenrod2", 61166, 46260, 8738);
	Enter_D (d, "goldenrod3", 52685, 39835, 7453);
	Enter_D (d, "goldenrod4", 35723, 26985, 5140);
	Enter_D (d, "gray", 32382, 32382, 32382);
	Enter_D (d, "gray0", 0, 0, 0);
	Enter_D (d, "gray1", 771, 771, 771);
	Enter_D (d, "gray10", 6682, 6682, 6682);
	Enter_D (d, "gray100", 65535, 65535, 65535);
	Enter_D (d, "gray11", 7196, 7196, 7196);
	Enter_D (d, "gray12", 7967, 7967, 7967);
	Enter_D (d, "gray13", 8481, 8481, 8481);
	Enter_D (d, "gray14", 9252, 9252, 9252);
	Enter_D (d, "gray15", 9766, 9766, 9766);
	Enter_D (d, "gray16", 10537, 10537, 10537);
	Enter_D (d, "gray17", 11051, 11051, 11051);
	Enter_D (d, "gray18", 11822, 11822, 11822);
	Enter_D (d, "gray19", 12336, 12336, 12336);
	Enter_D (d, "gray2", 1285, 1285, 1285);
	Enter_D (d, "gray20", 13107, 13107, 13107);
	Enter_D (d, "gray21", 13878, 13878, 13878);
	Enter_D (d, "gray22", 14392, 14392, 14392);
	Enter_D (d, "gray23", 15163, 15163, 15163);
	Enter_D (d, "gray24", 15677, 15677, 15677);
	Enter_D (d, "gray25", 16448, 16448, 16448);
	Enter_D (d, "gray26", 16962, 16962, 16962);
	Enter_D (d, "gray27", 17733, 17733, 17733);
	Enter_D (d, "gray28", 18247, 18247, 18247);
	Enter_D (d, "gray29", 19018, 19018, 19018);
	Enter_D (d, "gray3", 2056, 2056, 2056);
	Enter_D (d, "gray30", 19789, 19789, 19789);
	Enter_D (d, "gray31", 20303, 20303, 20303);
	Enter_D (d, "gray32", 21074, 21074, 21074);
	Enter_D (d, "gray33", 21588, 21588, 21588);
	Enter_D (d, "gray34", 22359, 22359, 22359);
	Enter_D (d, "gray35", 22873, 22873, 22873);
	Enter_D (d, "gray36", 23644, 23644, 23644);
	Enter_D (d, "gray37", 24158, 24158, 24158);
	Enter_D (d, "gray38", 24929, 24929, 24929);
	Enter_D (d, "gray39", 25443, 25443, 25443);
	Enter_D (d, "gray4", 2570, 2570, 2570);
	Enter_D (d, "gray40", 26214, 26214, 26214);
	Enter_D (d, "gray41", 26985, 26985, 26985);
	Enter_D (d, "gray42", 27499, 27499, 27499);
	Enter_D (d, "gray43", 28270, 28270, 28270);
	Enter_D (d, "gray44", 28784, 28784, 28784);
	Enter_D (d, "gray45", 29555, 29555, 29555);
	Enter_D (d, "gray46", 30069, 30069, 30069);
	Enter_D (d, "gray47", 30840, 30840, 30840);
	Enter_D (d, "gray48", 31354, 31354, 31354);
	Enter_D (d, "gray49", 32125, 32125, 32125);
	Enter_D (d, "gray5", 3341, 3341, 3341);
	Enter_D (d, "gray50", 32639, 32639, 32639);
	Enter_D (d, "gray51", 33410, 33410, 33410);
	Enter_D (d, "gray52", 34181, 34181, 34181);
	Enter_D (d, "gray53", 34695, 34695, 34695);
	Enter_D (d, "gray54", 35466, 35466, 35466);
	Enter_D (d, "gray55", 35980, 35980, 35980);
	Enter_D (d, "gray56", 36751, 36751, 36751);
	Enter_D (d, "gray57", 37265, 37265, 37265);
	Enter_D (d, "gray58", 38036, 38036, 38036);
	Enter_D (d, "gray59", 38550, 38550, 38550);
	Enter_D (d, "gray6", 3855, 3855, 3855);
	Enter_D (d, "gray60", 39321, 39321, 39321);
	Enter_D (d, "gray61", 40092, 40092, 40092);
	Enter_D (d, "gray62", 40606, 40606, 40606);
	Enter_D (d, "gray63", 41377, 41377, 41377);
	Enter_D (d, "gray64", 41891, 41891, 41891);
	Enter_D (d, "gray65", 42662, 42662, 42662);
	Enter_D (d, "gray66", 43176, 43176, 43176);
	Enter_D (d, "gray67", 43947, 43947, 43947);
	Enter_D (d, "gray68", 44461, 44461, 44461);
	Enter_D (d, "gray69", 45232, 45232, 45232);
	Enter_D (d, "gray7", 4626, 4626, 4626);
	Enter_D (d, "gray70", 46003, 46003, 46003);
	Enter_D (d, "gray71", 46517, 46517, 46517);
	Enter_D (d, "gray72", 47288, 47288, 47288);
	Enter_D (d, "gray73", 47802, 47802, 47802);
	Enter_D (d, "gray74", 48573, 48573, 48573);
	Enter_D (d, "gray75", 49087, 49087, 49087);
	Enter_D (d, "gray76", 49858, 49858, 49858);
	Enter_D (d, "gray77", 50372, 50372, 50372);
	Enter_D (d, "gray78", 51143, 51143, 51143);
	Enter_D (d, "gray79", 51657, 51657, 51657);
	Enter_D (d, "gray8", 5140, 5140, 5140);
	Enter_D (d, "gray80", 52428, 52428, 52428);
	Enter_D (d, "gray81", 53199, 53199, 53199);
	Enter_D (d, "gray82", 53713, 53713, 53713);
	Enter_D (d, "gray83", 54484, 54484, 54484);
	Enter_D (d, "gray84", 54998, 54998, 54998);
	Enter_D (d, "gray85", 55769, 55769, 55769);
	Enter_D (d, "gray86", 56283, 56283, 56283);
	Enter_D (d, "gray87", 57054, 57054, 57054);
	Enter_D (d, "gray88", 57568, 57568, 57568);
	Enter_D (d, "gray89", 58339, 58339, 58339);
	Enter_D (d, "gray9", 5911, 5911, 5911);
	Enter_D (d, "gray90", 58853, 58853, 58853);
	Enter_D (d, "gray91", 59624, 59624, 59624);
	Enter_D (d, "gray92", 60395, 60395, 60395);
	Enter_D (d, "gray93", 60909, 60909, 60909);
	Enter_D (d, "gray94", 61680, 61680, 61680);
	Enter_D (d, "gray95", 62194, 62194, 62194);
	Enter_D (d, "gray96", 62965, 62965, 62965);
	Enter_D (d, "gray97", 63479, 63479, 63479);
	Enter_D (d, "gray98", 64250, 64250, 64250);
	Enter_D (d, "gray99", 64764, 64764, 64764);
	Enter_D (d, "green", 0, 65535, 0);
	Enter_D (d, "green1", 0, 65535, 0);
	Enter_D (d, "green2", 0, 61166, 0);
	Enter_D (d, "green3", 0, 52685, 0);
	Enter_D (d, "green4", 0, 35723, 0);
	Enter_D (d, "green yellow", 44461, 65535, 12079);
	Enter_D (d, "grey", 32382, 32382, 32382);
	Enter_D (d, "grey0", 0, 0, 0);
	Enter_D (d, "grey1", 771, 771, 771);
	Enter_D (d, "grey10", 6682, 6682, 6682);
	Enter_D (d, "grey100", 65535, 65535, 65535);
	Enter_D (d, "grey11", 7196, 7196, 7196);
	Enter_D (d, "grey12", 7967, 7967, 7967);
	Enter_D (d, "grey13", 8481, 8481, 8481);
	Enter_D (d, "grey14", 9252, 9252, 9252);
	Enter_D (d, "grey15", 9766, 9766, 9766);
	Enter_D (d, "grey16", 10537, 10537, 10537);
	Enter_D (d, "grey17", 11051, 11051, 11051);
	Enter_D (d, "grey18", 11822, 11822, 11822);
	Enter_D (d, "grey19", 12336, 12336, 12336);
	Enter_D (d, "grey2", 1285, 1285, 1285);
	Enter_D (d, "grey20", 13107, 13107, 13107);
	Enter_D (d, "grey21", 13878, 13878, 13878);
	Enter_D (d, "grey22", 14392, 14392, 14392);
	Enter_D (d, "grey23", 15163, 15163, 15163);
	Enter_D (d, "grey24", 15677, 15677, 15677);
	Enter_D (d, "grey25", 16448, 16448, 16448);
	Enter_D (d, "grey26", 16962, 16962, 16962);
	Enter_D (d, "grey27", 17733, 17733, 17733);
	Enter_D (d, "grey28", 18247, 18247, 18247);
	Enter_D (d, "grey29", 19018, 19018, 19018);
	Enter_D (d, "grey3", 2056, 2056, 2056);
	Enter_D (d, "grey30", 19789, 19789, 19789);
	Enter_D (d, "grey31", 20303, 20303, 20303);
	Enter_D (d, "grey32", 21074, 21074, 21074);
	Enter_D (d, "grey33", 21588, 21588, 21588);
	Enter_D (d, "grey34", 22359, 22359, 22359);
	Enter_D (d, "grey35", 22873, 22873, 22873);
	Enter_D (d, "grey36", 23644, 23644, 23644);
	Enter_D (d, "grey37", 24158, 24158, 24158);
	Enter_D (d, "grey38", 24929, 24929, 24929);
	Enter_D (d, "grey39", 25443, 25443, 25443);
	Enter_D (d, "grey4", 2570, 2570, 2570);
	Enter_D (d, "grey40", 26214, 26214, 26214);
	Enter_D (d, "grey41", 26985, 26985, 26985);
	Enter_D (d, "grey42", 27499, 27499, 27499);
	Enter_D (d, "grey43", 28270, 28270, 28270);
	Enter_D (d, "grey44", 28784, 28784, 28784);
	Enter_D (d, "grey45", 29555, 29555, 29555);
	Enter_D (d, "grey46", 30069, 30069, 30069);
	Enter_D (d, "grey47", 30840, 30840, 30840);
	Enter_D (d, "grey48", 31354, 31354, 31354);
	Enter_D (d, "grey49", 32125, 32125, 32125);
	Enter_D (d, "grey5", 3341, 3341, 3341);
	Enter_D (d, "grey50", 32639, 32639, 32639);
	Enter_D (d, "grey51", 33410, 33410, 33410);
	Enter_D (d, "grey52", 34181, 34181, 34181);
	Enter_D (d, "grey53", 34695, 34695, 34695);
	Enter_D (d, "grey54", 35466, 35466, 35466);
	Enter_D (d, "grey55", 35980, 35980, 35980);
	Enter_D (d, "grey56", 36751, 36751, 36751);
	Enter_D (d, "grey57", 37265, 37265, 37265);
	Enter_D (d, "grey58", 38036, 38036, 38036);
	Enter_D (d, "grey59", 38550, 38550, 38550);
	Enter_D (d, "grey6", 3855, 3855, 3855);
	Enter_D (d, "grey60", 39321, 39321, 39321);
	Enter_D (d, "grey61", 40092, 40092, 40092);
	Enter_D (d, "grey62", 40606, 40606, 40606);
	Enter_D (d, "grey63", 41377, 41377, 41377);
	Enter_D (d, "grey64", 41891, 41891, 41891);
	Enter_D (d, "grey65", 42662, 42662, 42662);
	Enter_D (d, "grey66", 43176, 43176, 43176);
	Enter_D (d, "grey67", 43947, 43947, 43947);
	Enter_D (d, "grey68", 44461, 44461, 44461);
	Enter_D (d, "grey69", 45232, 45232, 45232);
	Enter_D (d, "grey7", 4626, 4626, 4626);
	Enter_D (d, "grey70", 46003, 46003, 46003);
	Enter_D (d, "grey71", 46517, 46517, 46517);
	Enter_D (d, "grey72", 47288, 47288, 47288);
	Enter_D (d, "grey73", 47802, 47802, 47802);
	Enter_D (d, "grey74", 48573, 48573, 48573);
	Enter_D (d, "grey75", 49087, 49087, 49087);
	Enter_D (d, "grey76", 49858, 49858, 49858);
	Enter_D (d, "grey77", 50372, 50372, 50372);
	Enter_D (d, "grey78", 51143, 51143, 51143);
	Enter_D (d, "grey79", 51657, 51657, 51657);
	Enter_D (d, "grey8", 5140, 5140, 5140);
	Enter_D (d, "grey80", 52428, 52428, 52428);
	Enter_D (d, "grey81", 53199, 53199, 53199);
	Enter_D (d, "grey82", 53713, 53713, 53713);
	Enter_D (d, "grey83", 54484, 54484, 54484);
	Enter_D (d, "grey84", 54998, 54998, 54998);
	Enter_D (d, "grey85", 55769, 55769, 55769);
	Enter_D (d, "grey86", 56283, 56283, 56283);
	Enter_D (d, "grey87", 57054, 57054, 57054);
	Enter_D (d, "grey88", 57568, 57568, 57568);
	Enter_D (d, "grey89", 58339, 58339, 58339);
	Enter_D (d, "grey9", 5911, 5911, 5911);
	Enter_D (d, "grey90", 58853, 58853, 58853);
	Enter_D (d, "grey91", 59624, 59624, 59624);
	Enter_D (d, "grey92", 60395, 60395, 60395);
	Enter_D (d, "grey93", 60909, 60909, 60909);
	Enter_D (d, "grey94", 61680, 61680, 61680);
	Enter_D (d, "grey95", 62194, 62194, 62194);
	Enter_D (d, "grey96", 62965, 62965, 62965);
	Enter_D (d, "grey97", 63479, 63479, 63479);
	Enter_D (d, "grey98", 64250, 64250, 64250);
	Enter_D (d, "grey99", 64764, 64764, 64764);
	Enter_D (d, "honeydew", 61680, 65535, 61680);
	Enter_D (d, "honeydew1", 61680, 65535, 61680);
	Enter_D (d, "honeydew2", 57568, 61166, 57568);
	Enter_D (d, "honeydew3", 49601, 52685, 49601);
	Enter_D (d, "honeydew4", 33667, 35723, 33667);
	Enter_D (d, "hot pink", 65535, 26985, 46260);
	Enter_D (d, "indian red", 27499, 14649, 14649);
	Enter_D (d, "ivory", 65535, 65535, 61680);
	Enter_D (d, "ivory1", 65535, 65535, 61680);
	Enter_D (d, "ivory2", 61166, 61166, 57568);
	Enter_D (d, "ivory3", 52685, 52685, 49601);
	Enter_D (d, "ivory4", 35723, 35723, 33667);
	Enter_D (d, "khaki", 46003, 46003, 32382);
	Enter_D (d, "khaki1", 65535, 63222, 36751);
	Enter_D (d, "khaki2", 61166, 59110, 34181);
	Enter_D (d, "khaki3", 52685, 50886, 29555);
	Enter_D (d, "khaki4", 35723, 34438, 20046);
	Enter_D (d, "lavender", 59110, 59110, 64250);
	Enter_D (d, "lavender blush", 65535, 61680, 62965);
	Enter_D (d, "lawn green", 31868, 64764, 0);
	Enter_D (d, "lemon chiffon", 65535, 64250, 52685);
	Enter_D (d, "light blue", 45232, 58082, 65535);
	Enter_D (d, "light coral", 61680, 32896, 32896);
	Enter_D (d, "light cyan", 57568, 65535, 65535);
	Enter_D (d, "light goldenrod", 61166, 56797, 33410);
	Enter_D (d, "light goldenrod yellow", 64250, 64250, 53970);
	Enter_D (d, "light gray", 43176, 43176, 43176);
	Enter_D (d, "light grey", 43176, 43176, 43176);
	Enter_D (d, "light pink", 65535, 46774, 49601);
	Enter_D (d, "light salmon", 65535, 41120, 31354);
	Enter_D (d, "light sea green", 8224, 45746, 43690);
	Enter_D (d, "light sky blue", 34695, 52942, 64250);
	Enter_D (d, "light slate blue", 33924, 28784, 65535);
	Enter_D (d, "light slate gray", 30583, 34952, 39321);
	Enter_D (d, "light slate grey", 30583, 34952, 39321);
	Enter_D (d, "light steel blue", 31868, 39064, 54227);
	Enter_D (d, "light yellow", 65535, 65535, 57568);
	Enter_D (d, "lime green", 0, 44975, 5140);
	Enter_D (d, "linen", 64250, 61680, 59110);
	Enter_D (d, "magenta", 65535, 0, 65535);
	Enter_D (d, "magenta1", 65535, 0, 65535);
	Enter_D (d, "magenta2", 61166, 0, 61166);
	Enter_D (d, "magenta3", 52685, 0, 52685);
	Enter_D (d, "magenta4", 35723, 0, 35723);
	Enter_D (d, "maroon", 36751, 0, 21074);
	Enter_D (d, "maroon1", 65535, 13364, 46003);
	Enter_D (d, "maroon2", 61166, 12336, 42919);
	Enter_D (d, "maroon3", 52685, 10537, 37008);
	Enter_D (d, "maroon4", 35723, 7196, 25186);
	Enter_D (d, "medium aquamarine", 0, 37779, 36751);
	Enter_D (d, "medium blue", 12850, 12850, 52428);
	Enter_D (d, "medium forest green", 12850, 33153, 19275);
	Enter_D (d, "medium goldenrod", 53713, 49601, 26214);
	Enter_D (d, "medium orchid", 48573, 21074, 48573);
	Enter_D (d, "medium purple", 37779, 28784, 56283);
	Enter_D (d, "medium sea green", 13364, 30583, 26214);
	Enter_D (d, "medium slate blue", 27242, 27242, 36237);
	Enter_D (d, "medium spring green", 8995, 36494, 8995);
	Enter_D (d, "medium turquoise", 0, 53970, 53970);
	Enter_D (d, "medium violet red", 54741, 8224, 31097);
	Enter_D (d, "midnight blue", 12079, 12079, 25700);
	Enter_D (d, "mint cream", 62965, 65535, 64250);
	Enter_D (d, "misty rose", 65535, 58596, 57825);
	Enter_D (d, "moccasin", 65535, 58596, 46517);
	Enter_D (d, "navajo white", 65535, 57054, 44461);
	Enter_D (d, "navy", 8995, 8995, 30069);
	Enter_D (d, "navy blue", 8995, 8995, 30069);
	Enter_D (d, "old lace", 65021, 62965, 59110);
	Enter_D (d, "olive drab", 27499, 36494, 8995);
	Enter_D (d, "orange", 65535, 34695, 0);
	Enter_D (d, "orange1", 65535, 42405, 0);
	Enter_D (d, "orange2", 61166, 39578, 0);
	Enter_D (d, "orange3", 52685, 34181, 0);
	Enter_D (d, "orange4", 35723, 23130, 0);
	Enter_D (d, "orange red", 65535, 17733, 0);
	Enter_D (d, "orchid", 61423, 33924, 61423);
	Enter_D (d, "orchid1", 65535, 33667, 64250);
	Enter_D (d, "orchid2", 61166, 31354, 59881);
	Enter_D (d, "orchid3", 52685, 26985, 51657);
	Enter_D (d, "orchid4", 35723, 18247, 35209);
	Enter_D (d, "pale goldenrod", 61166, 59624, 43690);
	Enter_D (d, "pale green", 29555, 57054, 30840);
	Enter_D (d, "pale turquoise", 44975, 61166, 61166);
	Enter_D (d, "pale violet red", 56283, 28784, 37779);
	Enter_D (d, "papaya whip", 65535, 61423, 54741);
	Enter_D (d, "peach puff", 65535, 56026, 47545);
	Enter_D (d, "peru", 52685, 34181, 16191);
	Enter_D (d, "pink", 65535, 46517, 50629);
	Enter_D (d, "pink1", 65535, 46517, 50629);
	Enter_D (d, "pink2", 61166, 43433, 47288);
	Enter_D (d, "pink3", 52685, 37265, 40606);
	Enter_D (d, "pink4", 35723, 25443, 27756);
	Enter_D (d, "plum", 50629, 18504, 39835);
	Enter_D (d, "plum1", 65535, 48059, 65535);
	Enter_D (d, "plum2", 61166, 44718, 61166);
	Enter_D (d, "plum3", 52685, 38550, 52685);
	Enter_D (d, "plum4", 35723, 26214, 35723);
	Enter_D (d, "powder blue", 45232, 57568, 59110);
	Enter_D (d, "purple", 41120, 8224, 61680);
	Enter_D (d, "purple1", 39835, 12336, 65535);
	Enter_D (d, "purple2", 37265, 11308, 61166);
	Enter_D (d, "purple3", 32125, 9766, 52685);
	Enter_D (d, "purple4", 21845, 6682, 35723);
	Enter_D (d, "red", 65535, 0, 0);
	Enter_D (d, "red1", 65535, 0, 0);
	Enter_D (d, "red2", 61166, 0, 0);
	Enter_D (d, "red3", 52685, 0, 0);
	Enter_D (d, "red4", 35723, 0, 0);
	Enter_D (d, "rosy brown", 48316, 36751, 36751);
	Enter_D (d, "royal blue", 16705, 26985, 57825);
	Enter_D (d, "saddle brown", 35723, 17733, 4883);
	Enter_D (d, "salmon", 59881, 38550, 31354);
	Enter_D (d, "salmon1", 65535, 35980, 26985);
	Enter_D (d, "salmon2", 61166, 33410, 25186);
	Enter_D (d, "salmon3", 52685, 28784, 21588);
	Enter_D (d, "salmon4", 35723, 19532, 14649);
	Enter_D (d, "sandy brown", 62708, 42148, 24672);
	Enter_D (d, "sea green", 21074, 38293, 33924);
	Enter_D (d, "seashell", 65535, 62965, 61166);
	Enter_D (d, "seashell1", 65535, 62965, 61166);
	Enter_D (d, "seashell2", 61166, 58853, 57054);
	Enter_D (d, "seashell3", 52685, 50629, 49087);
	Enter_D (d, "seashell4", 35723, 34438, 33410);
	Enter_D (d, "sienna", 38550, 21074, 11565);
	Enter_D (d, "sienna1", 65535, 33410, 18247);
	Enter_D (d, "sienna2", 61166, 31097, 16962);
	Enter_D (d, "sienna3", 52685, 26728, 14649);
	Enter_D (d, "sienna4", 35723, 18247, 9766);
	Enter_D (d, "sky blue", 29298, 40863, 65535);
	Enter_D (d, "slate blue", 32382, 34952, 43947);
	Enter_D (d, "slate gray", 28784, 32896, 37008);
	Enter_D (d, "slate grey", 28784, 32896, 37008);
	Enter_D (d, "snow", 65535, 64250, 64250);
	Enter_D (d, "snow1", 65535, 64250, 64250);
	Enter_D (d, "snow2", 61166, 59881, 59881);
	Enter_D (d, "snow3", 52685, 51657, 51657);
	Enter_D (d, "snow4", 35723, 35209, 35209);
	Enter_D (d, "spring green", 16705, 44204, 16705);
	Enter_D (d, "steel blue", 21588, 28784, 43690);
	Enter_D (d, "tan", 57054, 47288, 34695);
	Enter_D (d, "tan1", 65535, 42405, 20303);
	Enter_D (d, "tan2", 61166, 39578, 18761);
	Enter_D (d, "tan3", 52685, 34181, 16191);
	Enter_D (d, "tan4", 35723, 23130, 11051);
	Enter_D (d, "thistle", 55512, 49087, 55512);
	Enter_D (d, "thistle1", 65535, 57825, 65535);
	Enter_D (d, "thistle2", 61166, 53970, 61166);
	Enter_D (d, "thistle3", 52685, 46517, 52685);
	Enter_D (d, "thistle4", 35723, 31611, 35723);
	Enter_D (d, "tomato", 65535, 25443, 18247);
	Enter_D (d, "tomato1", 65535, 25443, 18247);
	Enter_D (d, "tomato2", 61166, 23644, 16962);
	Enter_D (d, "tomato3", 52685, 20303, 14649);
	Enter_D (d, "tomato4", 35723, 13878, 9766);
	Enter_D (d, "transparent", 0, 0, 257);
	Enter_D (d, "turquoise", 6425, 52428, 57311);
	Enter_D (d, "turquoise1", 0, 62965, 65535);
	Enter_D (d, "turquoise2", 0, 58853, 61166);
	Enter_D (d, "turquoise3", 0, 50629, 52685);
	Enter_D (d, "turquoise4", 0, 34438, 35723);
	Enter_D (d, "violet", 40092, 15934, 52942);
	Enter_D (d, "violet red", 62451, 15934, 38550);
	Enter_D (d, "wheat", 62965, 57054, 46003);
	Enter_D (d, "wheat1", 65535, 59367, 47802);
	Enter_D (d, "wheat2", 61166, 55512, 44718);
	Enter_D (d, "wheat3", 52685, 47802, 38550);
	Enter_D (d, "wheat4", 35723, 32382, 26214);
	Enter_D (d, "white", 65535, 65535, 65535);
	Enter_D (d, "white smoke", 62965, 62965, 62965);
	Enter_D (d, "yellow", 65535, 65535, 0);
	Enter_D (d, "yellow1", 65535, 65535, 0);
	Enter_D (d, "yellow2", 61166, 61166, 0);
	Enter_D (d, "yellow3", 52685, 52685, 0);
	Enter_D (d, "yellow4", 35723, 35723, 0);
	Enter_D (d, "yellow green", 12850, 55512, 14392);
}


const	AbMapping(String, Color)&	GetNamedColors ()
{
	static	Mapping_HashTable(String, Color)*	sMapping = Nil;
	if (sMapping == Nil) {
		sMapping = new Mapping_HashTable(String, Color) (&DefaultStringHashFunction, 101);
		AssertNotNil (sMapping);
		FillMapping1 (*sMapping);
		FillMapping2 (*sMapping);
	}
	EnsureNotNil (sMapping);
	return (*sMapping);
}






/*
 ********************************************************************************
 ********************************** StringToColor *******************************
 ********************************************************************************
 */
Color	StringToColor (const String& colorName)
{
	Require (colorName != kEmptyString);

	Color	c;
	if (GetNamedColors ().Lookup (colorName, &c)) {
		return (c);
	}
	else {
		// default is to assume its a #RRRRGGGGBBBBB format name.
		// If it was a named color we didn't recognize, oh well, issue a warning, and use black...
		if ((colorName[1] == '#') and (colorName.GetLength () == 1+3*4)) {
			ColorComponent	red		=	0;
			ColorComponent	green	=	0;
			ColorComponent	blue	=	0;
			red = strtoul (colorName.SubString (2, 4), 16);
			green = strtoul (colorName.SubString (6, 4), 16);
			blue = strtoul (colorName.SubString (10, 4), 16);
			return (Color (red, green, blue));
		}
		else {
#if		qDebug
			gDebugStream << "Unknown color " << colorName << " using black." << newline;
#endif
			return (kBlackColor);
		}
	}
}

String	ColorToXFormatColorName (Color c)
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MapElement(String, Color), it, GetNamedColors ().operator Iterator(MapElement(String, Color))* ()) {
#else
	ForEach (MapElement(String, Color), it, GetNamedColors ()) {
#endif
		if (it.Current ().fElt == c) {
			return (it.Current ().fKey);
		}
	}
	
	String	red		=	ultostring (c.GetRed (), 16);
	String	green	=	ultostring (c.GetGreen (), 16);
	String	blue	=	ultostring (c.GetBlue (), 16);
	while (red.GetLength () < 4) { red = String ("0") + red; };
	while (green.GetLength () < 4) { green = String ("0") + green; };
	while (blue.GetLength () < 4) { blue = String ("0") + blue; };
	return (String ("#") + red + green + blue);
}



