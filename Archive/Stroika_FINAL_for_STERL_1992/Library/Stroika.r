

#include	"StroikaVersion"

resource 'vers' (128) {
	kStroikaMajorVersion,
	kStroikaMinorVersion,
	kStroikaStage,
	kStroikaStageVersion,
	verUS,
	kStroikaShortVersionString,
	kStroikaLongVersionString "\n" $$Date "\n"
};



#define	kAssureThatAlert	1024

resource 'ALRT' (kAssureThatAlert) {
	{66, 88, 252, 410},
	kAssureThatAlert,
	{
		Cancel, visible, sound1,
		Cancel, visible, sound1,
		Cancel, visible, sound1,
		Cancel, visible, sound1
	},
#if		SystemSevenOrLater
	alertPositionMainScreen
#endif
};

resource 'DITL' (kAssureThatAlert) {
	{
		/* [1] */	{22, 59, 125, 292},		StaticText	{	disabled,	"^0"	},
		/* [2] */	{141, 132, 161, 190},	Button 		{	enabled,	"Abort"	}
	}
};



#define	rGetDirectoryDLOG	1002
// need a better way to organize this shit!!!
// resources for the GetDIR old pathname selector
resource 'DLOG' (rGetDirectoryDLOG, purgeable) {
	{0, 0, 217, 348},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	rGetDirectoryDLOG,
	"",
#if		SystemSevenOrLater
	centerMainScreen
#endif
};


resource 'DITL' (rGetDirectoryDLOG, purgeable) {
	{	/* array DITLarray: 13 elements */
/* [1] */	{142, 256, 160, 336},	Button		{enabled,"Open"},
/* [2] */	{1152, 59, 1232, 77},	Button		{enabled,"Hidden"},
/* [3] */	{193, 256, 211, 336},	Button		{enabled,"Cancel"},
/* [4] */	{43, 232, 63, 347},		UserItem	{disabled},
/* [5] */	{72, 256, 90, 336},		Button		{enabled,"Eject"},
/* [6] */	{97, 256, 115, 336},	Button		{enabled,"Drive"},
/* [7] */	{43, 12, 189, 230},		UserItem	{enabled},
/* [8] */	{43, 229, 189, 246},	UserItem	{enabled},
/* [9] */	{128, 252, 129, 340},	UserItem	{disabled},
/* [10] */	{1044, 20, 1145, 116},	StaticText	{disabled,""},
/* [11] */	{167, 256, 185, 336},	Button		{enabled,"Directory"},
/* [12] */	{0, 30, 18, 215},		Button		{enabled,"Select Current Directory:"},
/* [13] */	{200, 20, 1145, 116},	StaticText	{disabled,""}
	}
};


