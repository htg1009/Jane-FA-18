//*****************************************************************************************
//  BEHAVE.H
//*****************************************************************************************

//Need to forward declare this, otherwise some things won't work right
class PlaneParams;  

//  DEFINES ****************************************************************************
#define NOTHING 0
#define LASTWAY 1001		//  Used during prototyping of mission builder to identify the last waypoint in a path.
#define GOBACKX 1002		//  Used during prototyping of mission builder to loop the waypoint list.
//**************************************************************************************
//  MBUILDER.H   
//**************************************************************************************

#define MISSIONID	900  //  1
#define SCENARIOID	900  //  1

//  iunique ID in various structures use these flags.
#define MBISMOVE		0x20000000		//  If &ed with a unique ID identifies that this is a moving object. 
#define MBISSCENARIO	0x40000000		//  If &ed with a unique ID identifies that this is a plug-in object.
#define MBISWORLD		0x80000000		//  If &ed with a unique ID identifies that this is a world object.


//  lHideMoveObjList array uses these bit flags.
#define MBHIDEALL			0x1			//  If bit is set then user has requested to hide all objects.
#define MBHIDESIDE		0x2				//  If this bit has been set then the user has requested objects of this side be hidden.
#define MBHIDECLASS		0x4				//  If this bit has been set then ths user has requested that objects of this class be hidden (moving, plug-in,...)
#define MBHIDEPATH		0x8				//  If this bit has been set then the user has asked that the paths for this object be hidden.
#define MBHIDEIOBJ		0x10			//  If this bit has been set then the user has asked that this specific object be hidden.


//  MoveStatus uses these bit flags.
#define MBPATHFIRSTWP	0x1				//  If this bit is set then the moving/added waypoint is the first waypoint of a path.
#define MBPATHLASTWP	0x2				//  If this bit is set then the moving/added waypoint is the last waypoint of a path.
#define MBADDPATHDRAWN  0x4				//  If this bit is set then the path being edited/added to has been drawn (we only do this once for better framerate).
#define MBNEWALTPATH	0x8				//  If this bit is set then we are adding the first waypoint of an alternate path.  Use waypoint icon instead of object icon since not a new object.
#define MBOEVENTALTPATH 0x10			//  If this bit is set then we are adding an alternate path from editing an object event.  This lets us return to doing this once the path is added.


//  lMBMainShow uses these bit flags.
#define MBSHOWLATLONG   0x1				//  If this bit is set then we can show the Lat/Long box.
#define MBREDOLATLONG   0x2				//  If this bit is set then redraw the Lat/Long box boarder.
#define MBNEEDLATLONG	0x4				//  If this bit is set then force Lat/Long box to draw.  Used when selecting an object from the map.
#define MBMENUSTAYSET	0x8				//  If this bit is set then if the Cancel button/Esc key is hit then we will Null the MenuFunc but keep SubMenu where is was (not go up one level).
#define MBREDOMAPOPT    0x10			//  If this bit is set then redraw the Map Option box boarder.
#define MBMAPOPTLBDOWN  0x20			//  If this bit is set then we are waiting on a map option to be finished so don't redraw the terrain map.
#define MBBALLOKCLICK   0x40			//  If this bit is set then the OK button of the ball has been clicked.
#define MBBALLCANCELCLICK 0x80			//  If this bit is set then the Cancel button of the ball has been clicked.
#define MBBALLHELPCLICK 0x100			//  if this bit is set then the Help button of the ball has been clicked.



//  Color definitions.
#define MBBLACK 0		//  BLACK
#define MBBACKCOLOR 1  // GREY
#define MBWHITE 15		//  WHITE
#define MBHOTCOLOR 10		//  PINK
#define MBSTARTCOLOR 20		// GREEN
#define MBNOSELECTCOLOR 25	//  DARK GREY
#define MBLINKCOLOR 7	//  LIGHT BLUE


//  General Definitions.
#define MBF18ETYPE		61		//  Number representing plane type is F-18, used to keep humans from being something other then F-15.
#define MAXAIOBJS 80			//  Maximum number of moving objects (planes, vehicles(convoys)).  Also maximum number of paths (Should be less paths then objects since a group of planes can use one path, and alternate paths are the only way a group can have more than one path.
#define MAXSTATICOBJS 100		//  Maximum number of plug-ins.
#define MAXWPTS 200				//  Maximum number of waypoints.
#define MAXACTS 400				//  Maximum number of actions.
#define TITLETEXTLEN 60			//  Maximum string length of title.
#define SHORTTEXTLEN 400		//  Maximum string length of short briefing.
#define MAXTEXTLEN 2000			//  Maximum string length of briefing.
#define MENUTEXTSIZE 80			//  Maximum string length for a menu item.
#define NAMESIZE   12			//  Maximum string length for a name used to help ID planes.
#define NUMSIDES   10			//  Maximum number of sides in a mission.
#define MESSAGELENGTH 120		//  Maximum string length for the text of a mission.
#define FNAMESIZE  15			//  Maximum string length for a file name to be stored.
#define AFILENAME  12			//  Maximum string length when getting a file name.
#define MAXMESSAGES 20			//  Maximum number of messages that can be used in a mission (array size).
#define GOALMESSAGESIZE 800		//  Maximum string length for briefing messages gotten from goals.
#define NUMGOALS	20			//  Maximum number of goals (array size).
#define NUMOBJEVENTS	20		//  Maximum number of object events (array size).
#define MAXMENUITEMS 160		//  Maximum number of items in a menu (array size).
#define NUMORD 4				//  Maximum number of different ordinace loads on a plane (probably change).
#define MISSIONCHECKNUM 437		//  Used to confirm that a file is a mission file.
#define SCENARIOCHECKNUM 434	//  Used to confirm that a file is a scenario file (This will probably not be needed except by old missions since we got rid of this)
#define LONGFILENAME 60			//  Maximum number of characters that can be used for a long file name.
#define PASSSIZE	12			//  Maximum number of characters that can be used for a password for Scenario files (see scenariochecknum)
#define MBBUTTONWIDTH 132		//  Width of OK/Cancel Buttons
#define MBBUTTONHEIGHT 28		//  Height of OK/Cancel Buttons
#define MBCHNGARROWLRWIDTH 8	//  Width of left/right change arrows (for changing values like map scale).
#define MBCHNGARROWLRHEIGHT 14	//  Height of left/right change arrows.
#define MBCHNGARROWUDWIDTH 14	//  Width of up/down change arrows (for things like changing numbers).
#define MBCHNGARROWUDHEIGHT 8	//  Height of up/down change arrows.
#define MBICONWIDTH 25			//  Width of object/waypoint icons.
#define MBICONHEIGHT 25			//  Height of object/waypoint icons.
#define MBCURSORINC 0  //  used when determining space needed for cursor when determining line length  was 2
#define MBSHOWNCURSORINC 2  //  was 2	//  Space to use when showing cursor, but not determining line length.

#define MAPSCREENX0 11		//  Left screen coordinate of map area.
#define MAPSCREENZ0 8		//  Top screen coordinate of map area.
#define MAPSCREENX 460 //  480	//  Width of map area in screen coordinates.
#define MAPSCREENZ 460 //  480	//  Height of map area in screen coordinates.


//  iWptStatus can be set to these defines.
#define NOWPACTION -1		//  Identifies that no waypoint action is taking place.
#define CLEARWPS 1			//  Identifies that we want to clear the mission data.
#define NEWWP 2				//  Identifies that we are adding a new waypoint path and object.
#define ADDWP 3				//  Identifies that we are adding a new waypoint to an existing path.
#define MOVEWP 4			//  Identifies that we are moving a waypoing.
#define REFRESHWPS 5		//  Identifies that we want to redraw waypoints.
#define MBZOOMBOX 6			//  Identifies that we are going to be drawing a zoom box (new area we the map to show).
#define MBZOOMBOXC 7		//  Identifies that we are going to be drawing a zoom box with the center being where the mouse if first clicked.
#define MBMENUMOVE 8		//  Identifies that a menu selection has been made where the player wants to move a waypoints (if first of path then moving plane as well).
#define DOREFRESH 11		//  Identifies that we want to redraw waypoints.  This is done when we need a delay before we redraw the waypoints.  DOREFRESH switches to REFRESHWPS during keyboard check.


//  A couple of defines for menus.
#define MENUSTAY 11			//  Return value that informs us to keep the SubMenu where it is at
#define MENUNOSELECT 9999	//  Indetifies that a menu item is not selectable.


//  iBuilderStatus can be set to these defines.
#define GETFILETEXT 8		//  Identifies that we are currently getting a file name.
#define GETREADFILE 9		//  Identifies that we are getting a file name we want to load.
#define GETWRITEFILE 10		//  Identifies that we are getting a file name we want to write.


//  iflag in some structures can use these bit flags (like AIObjects).
#define AL_ACTIVE 1			//  Identifies that an object is active/being used.
#define AL_AI_DRIVEN 2		//  Identifies that an object is an AI.
#define AL_DEVICE_DRIVEN 4	//  Identifies that an object is device driven.
#define AL_COMM_DRIVEN 8	//  Identifies that an object is a network player.
#define AL_ISVEHICLE 16		//  Identifies that an object is a ground vehicle.
#define AL_HIDE_IN_BRIEFING 32	//  Not used yet, but can be used to identify that an object should be hidden in the mission briefing map.


//  Defines for menus and editing functions.
#define OBJECTMENUID   1				//  Menu ID representing object editing menu (Add/edit/delete a plane/vehicle/plug-in.
#define PLANEMENUID    2				//  Menu ID representing we are editing a plane.
#define WAYPOINTMENUID 3				//  Menu ID representing we are editing a waypoint. 
#define ACTIONMENUID   4				//  Menu ID representing we selecting an action to add.
#define ACTIONEDMENU   5				//  Not sure if this is being used anymore, can find it in one case statment, but not anywhere where it is set.
#define VEHICLEMENUID  6				//  Menu ID representing we are editing a vehicle.
#define MAPSHOWMENUID  7				//  Menu ID representing we are modifying what is shown on the map.
#define HSCLASSMENUID  8				//  Menu ID representing we are getting which class (moving, plug-in) we want to hide/show on the map.
#define PLANECONTROLMENUID 10			//  Menu ID representing we are selecting the control type for an object.
#define RANDOMMENUID	15				//  Menu ID representing we are setting up the random appearance constraints for an object.
#define MISSIONMENUID  20				//  Menu ID representing we are editing the more global mission data (briefings, weather, time of day, ...)
#define FILEMISSIONMENUID	21			//  Menu ID representing we are getting ready to save, load, or create a mission.
#define MISSWORLDMENUID	22				//  Menu ID representing we are selecting a new world to make a mission for (or just starting a new mission).
#define LOADFILEMENUID	30				//  Menu ID representing we are getting a filename to read from.
#define SAVEFILEMENUID	31				//  Menu ID representing we are getting a filename to write to.
#define SCENARIOMENUID  50				//  Menu ID representing we are going to change the scenario file we are loading (old mission format) or going to edit the plug-in data (possibly requireing password).  
#define ESCENARIOMENUID 55				//  Menu ID representing we are going to add, edit, or remove a plug-in.
#define ESCENOBJSMENUID 56				//  Menu ID representing we are going to modify information about a plug-in.
#define STATICLISTMENUID  60			//  Menu ID representing we are going to save a plug-in type.  (STATICINITMENUID becomes STATICLISTMENUID)
#define STATICINITMENUID  61			//  Menu ID representing we are going to pick a new plug in, or change its type. 
#define PLANELISTMENUID 100				//  Menu ID representing we are going to change plane types. 
#define SIDEMENUID		101				//  Menu ID representing we are selecting a new side for an objet.
#define SIDESHOWMENUID		102			//  Menu ID representing we are going to save information about hiding/showing objects of a side on a map. (SIDESHOWINITMENUID becomes SIDESHOWMENUID).
#define SIDESHOWINITMENUID		103		//  Menu ID representing we are setting up to change the hide/show a side information for showing map objects.
#define VEHICLELISTMENUID  105			//  Menu ID representing we are getting a different vehicle type for an object. (VEHICLEINITMENUID becomes VEHICLELISTMENUID).
#define VEHICLEINITMENUID  106			//  Menu ID representing we are setting up to get a different vehicle type for an object.
#define MESSAGEMENUID	110				//  Menu ID representing we are getting a message for an event or action.
#define FORMATIONSMENUID 150			//  Menu ID representing we are getting a new formation for the formation action.
#define WEAPONDELVMENUID 200			//  Menu ID representing we are getting a new weapon delivery method for a bombing action.
#define DWEAPONDELVMENUID 201			//  Menu ID representing what we are supposed to do with the delivery method information.
#define TARGETTYPEMENUID 210			//  Menu ID representing we are getting a target type for a bombing mission.
#define DTARGETTYPEMENUID 211			//  Menu ID representing what we are supposed to do with the target type information.
#define GENTARGTYPEMENUID 220			//  Menu ID representing we are getting a target type for a non detailed bombing action (will probably go away).
#define GETGOALMENUID		300			//  Menu ID used to make sure menu options are valid while preparing to get the type of goal we are setting up (must protect, must destroy, ...).
#define DGETGOALMENUID		301			//  Menu ID representing we are getting the type of goal we are setting up (must protect, must destroy, ...).
#define PGETGOALMENUID		302			//  Menu/Function ID representing we are getting the object the goal is linked to.
#define WGETGOALMENUID		303			//  Menu/Function ID representing we are are getting the waypoint needed for the goal.
#define CGETGOALMENUID		304			//  Menu/Function ID representing we are getting the object the goal object must get close to for goal completion.
#define GETOBJEVENTMENUID   350			//  Menu ID used to make sure menu options are valid while preparing to get the type of object event (Obj destroyed, obj detected, ...).
#define DGETOBJEVENTMENUID  351			//  Menu ID representing we are getting the type of object event trigger (object destroyed, object detected, ...).
#define PGETOBJEVENTMENUID  352			//  Menu ID representing we are getting the object we watch for the event.
#define EGETOBJEVENTMENUID  353			//  Menu ID representing we are modifying data needed to trigger the event (how close must an object get).
#define AGETOBJEVENTMENUID  354			//  Menu ID representing we are getting the action this event triggers.
#define GETOBJEVENTSIDEMENUID 355		//  Menu ID representing we are getting the side an object needs to be to trigger an event.
#define EVENTMESSAGEMENUID	356			//  Menu ID representing we are getting the message for a message event action.
#define WEATHERMENUID		400			//  Menu ID representing we are getting the weather for this mission.
#define ROEMENUID			450			//  Menu ID representing we are determining what side we want to change the ROE for.
#define SIDEROEMENUID		451			//  Menu ID representing we are changing the ROE for a side (SIDEROEINITMENUID becomes SIDEROEMENUID).
#define SIDEROEINITMENUID	452			//  Menu ID representing we are preparing to change the ROE for a side.
#define ORDINANCE1MENUID	500			//  Menu ID representing we are getting the weapon type for ordinance slot 1.
#define ORDINANCE2MENUID	501			//  Menu ID representing we are getting the weapon type for ordinance slot 2.
#define ORDINANCE3MENUID	502			//  Menu ID representing we are getting the weapon type for ordinance slot 3.
#define ORDINANCE4MENUID	503			//  Menu ID representing we are getting the weapon type for ordinance slot 4.
#define GETORDINANCEMENUID	520			//  Menu ID representing we are getting ready to change either the weapon type or number of weapons for an ordinance load.
#define SETALTPATHMENUID    600			//  Function ID representing we are setting up to create a new alternate path.
#define GETPLANEATMENUID 1000			//  Menu ID representing we have a list of objects, one of which we want to select. 
#define GETWAYPTATMENUID 1001			//  Menu ID representing we have a list of waypoints, one of which we want to select.
#define GETNCWAYPTATMENUID 1002			//  Menu ID representing we have a list of waypoints, one of which we want to select, but when selected don't change any MenuCoreVals information.
#define MBCHECKSCENATMENUID 1010		//  Menu ID representing we have a list of plug-ins, one of which we want to select.



//  Structures  **********************************************************************
typedef struct stMBOrdinance {  //  Prototype.  Holds the weapons data for a moving vehicle.  This format may change or be removed, and instead be getting a "package" number weapons.
	int iWeaponID;				//  Holds the element number of the weapon in weapons array.
	int iWeaponCnt;				//  Holds how much of this weapon is loaded.
} MBOrdinance;


struct stAIObject {				//  Holds the data in the mission builder for a moving object.  This may change for a ground vehicle.
	long X;						//  Holds the starting X position of the object, in feet, from the upper left corner of the map.
	long Y;						//  Holds the starting Y position of the object in feet.
	long Z;						//  Holds the starting X position of the object, in feet, from the upper left corner of the map.
	int itype;					//  Tells what type of object this is.
	long luniqueID;				//  Holds a unique ID for each object.  John may want to switch this to 64 bits to have enough space to incorporate information about ground vehicles.
	int iflag1;					//  Used for bit flags, like control type.  Not yet full.
	int iSide;					//  Holds the value for which side this object is on (could be switched to country)
	int numgroup;				//  Tells how many objects are in this group/flight.
	int	iWPPathNum;				//  Tells which element in the AIWPPaths array that this object is going to follow.
	int iFuelLoad;				//  Tells how many gallons of fuel this object has.  May not be needed.
	MBOrdinance	Weapons[NUMORD];  //  Holds the weapon loadout for this object.
	int iRandomChance;			//  Tells what the change is this object is going to appear.
	int iRandomGroup;			//  Tells which random group this object belongs.  Random groups can be used to set up such things as a random CAP, where each object of this group number is flying a CAP mission but in a different area.  Only one element/object of a group is picked for a mission.
	int iRandomElement;			//  Tells which random element this object belongs.  Random elements are used to "attach" objects together within a random group so that if one appears they all appear.  For example, if a attack group is picked, you would want to have its cover appear as well.
	char sIDName[NAMESIZE];		//  Holds a text string that can be used to identify an object with in the mission builder.  This is just in the builder and has no effect "in game".
};

typedef struct stAIObject MBObject;

struct stAIObjectMID1 {  //  Used for old mission compatibility.  See stAIObject except where noted below.
	long X;
	long Y;
	long Z;
	int itype;
	int luniqueID;
	int flag1;
	int iSide;
	int numgroup;
	int	iStartWP;				//  Tells which element in AIWayPoints is the first waypoint for this object.
	int iNumWP;					//  Tells how many consecutive waypoints this object is supposed to follow.
	int iFuelLoad;
	MBOrdinance	Weapons[NUMORD];
	int iRandomChance;
	int iRandomGroup;
	int iRandomElement;
	char sIDName[NAMESIZE];
};

typedef struct stAIObjectMID1 MBObjectMID1;


struct stWPPaths {	//  Holds information that identify what are the waypoints an object is supposed to follow, and which object is going to use this path.
	long lObjLink;  //  This lets me know for which object group this is a path for.  Right now it give the array number into AIObjects, though could be leader's ID (Advantages to each)
	int	iStartWP;	//  Tells which element in AIWayPoints is the first waypoint for this object to follow.
	int iNumWP;		//  Tells how many consecutive waypoints this object is supposed to follow.
};

typedef struct stWPPaths MBWPPaths;

struct stWayPoints {	//  Holds information for where waypoints are located, and what actions are associated with them.
	long	lWPx;		//  Holds the X position of the waypoint in feet from the top left corner of the map.
	long	lWPy;		//  Holds the Y position of the waypoint in feet.
	long	lWPz;		//  Holds the Z position of the waypoint in feet from the top left corner of the map.
	int		iStartAct;	//  Tells which element in AIActions is the first action for this waypoint (-1 if no action).
	int		iNumActs;	//  Tells how many consecutive action slots are used by this waypoint (an action can use more than 1 action slot.  (0 if no actions)
};

typedef struct stWayPoints MBWayPoints;

struct stMBActions {	//  Holds information about actions an data about the actions.  Some actions can use more than one of these (Such as bombing actions).
	long	ActionID;   //  Identifies what action is supposed to take place (unless continuation of a previous action).
	long	ActionVal;	//  Holds data that helps define the action, such as identifying what new formation is supposed to be flown if the ActionID represents a new formation.
};

typedef struct stMBActions MBActions;

typedef struct _Behavior	//  This holds the structure the AI planes use "in game"
{
	void		(*Behaviorfunc)(PlaneParams *planepnt);		//  This is a pointer to the AI function the plane is currently using.
	void		(*OrgBehave)(PlaneParams *planepnt);		//  This holds the pointer to the AI function the plane was using before getting attacked, or something else which caused him to deviate from his last behavior.
	MBWayPoints	*CurrWay;						//  This is a pointer to the current waypoint that the obj is flying towards.
	FPoint		WayPosition;					//  This holds the World Position that the AI is flying towards.
	int			numwaypts;						//  This holds the number of waypoints left to fly.
	int			startwpts;						//  This holds the number of the first waypoint this plane was using when he started his mission.
	int			winglead;						//  This tells what object is the wingleader for this object (-1 if this plane is the wingleader).
	int			wingman;						//  This tells what object is the wingman for this object (-1 if he is the wingman).
	int			prevpair;						//  This tells what object is the wingleader for the previous wing pair (-1 represents this is the lead wingpair).
	int			nextpair;						//  This tells what object is the wingleader for the next wing pair (-1 if none).
	int			iSide;							//  This identifies what side (or country) this object belongs.
	int			iFuelLoad;						//  This identifies how much fuel this object is carrying (may not be needed).
	int			cnt1;							//  This is used as a generic count variable when an AI needs to keep track of something.
	ANGLE		DesiredRoll;					//  This holds the value of the roll the object is trying to obtain.
	ANGLE		DesiredPitch;					//  This holds the value of the pitch the object is trying to obtain.
	ANGLE		DesiredHeading;					//  This holds the value of the heading the object is trying to obtain.
	ANGLE		LastRoll;						//  This tells what was the last roll the object had (for various delta calculations).
	ANGLE		LastPitch;						//  This tells what was the last pitch the object had (for various delta calculations).
	ANGLE		LastHeading;					//  This tells what was the last heading the object had (for various delta calculations).
	float		LastSymetricalElevator;			//  This tells what was the last setting for the symetrical elevator that the object had (for various delta calculations).
	float		PitchTrim;						//  This is a trim that the AI uses for trimming the aircraft, during times like turns, where I don't want to goof up the ElevatorTrim which is trimmed for level flight.
	float		fGLimitMaxElev;					//  Holds a value for which we don't want the elevator to go past since it may Negative over G the plane.  If we are at this setting, and are not close to the G limit, then value will be relaxed.
	float		fGLimitMinElev;					//  Holds a value for which we don't want the elevator to go past since it may over G the plane.  If we are at this setting, and are not close to the G limit, then value will be relaxed.
	float		fLastGForce;					//  Holds the last G force on the plane.  This helps determine if we can relax the elevator limits on the plane.
	float		fDesiredG;						//  This identifies what G limit the plane is trying to reach.
	long		lDesiredSpeed;					//  This identifies what speed the plane is trying to reach.
	float		fLastVel;						//  This identifies what was the planes speed last frame.  Used for delta calculations.
	float		fAngleFromLead;					//  This holds how much off our angle is from the lead plane's.  Used for delta calculations to see if it is increasing or decreasing.
	PlaneParams *AirTarget;				//  Is a pointer to a plane we are attacking.
	PlaneParams *AirThreat;				//  Is a pointer to a plane we are being attacked by.
	MBOrdinance	Weapons[NUMORD];				//  Identifies what weapons this plane is carrying.
	FPoint		FormationPosition;				//  This holds the offset off the lead plane which is where this plane wants to be when flying formation.  If plane is lead, then it holds Sin and Cos info which will be used by other planes during formation calculation.
	FPoint		WorldFormationPos;				//  This holds the position in the world where a plane should be if he is in formation.
	long		lTimer1;						//  This is a generic timer used when the AI needs to keep track of elapsed time.  Counts down to 0 using DeltaTicks.
	int			iVar1;							//  This is a generic variable that can be used by various behaviors.
	int			iAIFlags1;						//  Holder for bit flags
	long		lAltCheckDelay;					//  Time until height is checked next
	long		lMinAltFt[4];					//  Holds the maximum value for the minimum altitude a plane can fly for the next 20 seconds (one variable for every 5 seconds).
	float		fDistToHeight[4];				//  Holds the distance needed to travel to reach this height
} Behavior;

typedef struct stMBMenuItem {	//  This holds the data used by a single menu element.
	long	lItemID;        //  This will hold an id number for the menu item.  This will allow us to determine what to do with an item when it is selected.  This will also allow us to add things between items in the list and not screw up existing mission files.
	char	sItemText[MENUTEXTSIZE];  //  This is the text that is shown on the menu.
	char	sMenuFile[FNAMESIZE];  //  This holds the name of the file holding menu information.  Example The waypoint menu might have a "Add Action" item.  The sMenuFile could be "actions.mnu" which is the menu file loaded when you want to add an action.
} MBMenuItem;

typedef struct stMBMenu {		//  This holds the information which identifies the location and selection information about a menu.
	long		lMenuID;		//  This will hold an id number which will allow me to know how to process the menu
	int			MinX;			//  Define the left edge of the active menu area in screen coordinates.
	int			MinZ;			//  Defines the top edge of the active menu area in screen coordinates.
	int			width;			//  Defines the width of the active menu area.
	int			height;			//  Defines the height of the active menu area.
	int			iHotItem;		//  Identifies what is the currently selected (single click) menu item.
	int			iTopItem;		//  Identifies what is the first menu item that appears on the screen.
	int			iNumItems;		//  Tells how many items are in the menu.
	int			lastMX;			//  When using a horizontal slide bar, this holds a value that tells where the last mouse position was.
	int			lastMZ;			//  When using a vertical slide bar, this holds a value that tells where the last mouse position was.
	int			idoredraw;		//  Identifies if a menu should be redrawn.  Not drawn every frame to help with framerate.
	DWORD		dwTimer;		//  Used as a generic timer for such thing as a delay for holding down the mouse on a scroll arrow.
	MBMenuItem	MenuItem[MAXMENUITEMS];		//  Holds the array of menu items.
} MBMenu;

typedef struct stMBInstructText {	//  Holds the data used when putting general instructions on the screen.
	int			iIsActive;			//  Identifies if there are instructions to be shown.
	char		sInstructions[400];	//  Holds the instruction text to be shown.
	int			MinX;				//  Define the left edge of the written instruction area in screen coordinates.
	int			MinZ;				//  Defines the top edge of the written instruction area in screen coordinates.
	int			width;				//  Defines the width of the written instruction area.                         
	int			height;				//  Defines the height of the written instruction area.                        
	int			color;				//  Was used when fonts needed a color.  Probably no longer needed, but many functions still are looking for the value to be passed.
	int			backcolor;			//  Was used for a background color before artwork was copied in.  Probably no longer needed, but many functions still are looking for the value to be passed.
	GrFontBuff  *font;				//  Is a pointer to the font that the instructions are going to use to print.
	int			idoredraw;			//  Identifies if a menu should be redrawn.  Not drawn every frame to help with framerate.
} MBInstructText;

typedef struct stMBMenuFunc {		//  This is the function used when we want to do things be sides using the standard menus, such as getting speeds, goals, or even using menus out of the MainMenu, SubMenu context.
	void (*MenuFunc)();				//  Points to the function we want to use.
	GrBuff *SREBuff;				//  Points to the screen buffer we want to use.
	long lArg1;						//  Generic first variable that can be used by function.
	long lArg2;						//  Generic second variable that can be used by function.
	long lArg3;						//  Generic third variable that can be used by function.
	long lArg4;						//  Generic fourth variable that can be used by function.
	long lArg5;						//  Generic fifth variable that can be used by function.
	long lArg6;						//  Generic sixth variable that can be used by function.
	long lArg7;						//  Generic seventh variable that can be used by function.
	long *lupdatevar;				//  Pointer to a long that is going to be updated by a funtion.
	int  *iupdatevar;				//  Pointer to an int that is going to be updated by a function (heading, speed, etc).
	long lreturnval;				//  Holds a return value for the function.
	int  mousex;					//  Holds the mouse X coordinate passed down from main loop.
	int  mousey;					//  Holds the mouse Y coordinate passed down from main loop.
	short mouselb;					//  Holds the mouse left button passed down from the main loop.
	short mouserb;					//  Holds the mouse right button passed down from the main loop.
	long lMenuID;  //  So we know how the final data for the function needs to be processed
	long lItemID;  //  Same.
	int  itopitem;					//  Holds the top item number when a slider might be employed, like editing goals, events, and things like that.
	int iSlideZ;					//  Is used to determine new slide position when slide bar is being moved around.
	DWORD  dwTimer;					//  Generic timer, mainly used for slide bar/arrow stuff.
	int idoredraw;					//  Identifies if a screen should be redraw.  Not always used (not sure if used at all right now).
} MBMenuFunc;

typedef struct stMBWorkingText {	//  Holds the data used by the generic text editing functions.
	int	iIsActive;					//  Identifies if or how this structure is being used.
	char sFrontText[MAXTEXTLEN];	//  Front half of text being edited.  Cursor represents area between front and back half of text, so that new text is added at end of front half.
	char sBackText[MAXTEXTLEN];		//  Back half of text, ie. text after cursor.
	int iCursorX;					//  Current cursor X position on screen.  Useful when using arrows to move cursor around.
	int iCursorZ;					//  Current cursor Z(Y) position on screen.  Useful when using arrows to move cursor around.
	int iSlideX;					//  Is used to determine new slide position when slide bar is being moved around horizontally.
	int iSlideZ;					//  Is used to determine new slide position when slide bar is being moved around vertically.
	int iMinX;						//  Left side of active editing area.
	int iMinZ;						//  Top of active editing area.
	int iwidth;						//  Width of active editing area.
	int iheight;					//  Height of active editing area.
	int iIsSlide;					//  Identifies if a slide bar is drawn.
	DWORD  dwTimer;					//  Timer used for slide bar stuff.
	int iMaxSize;					//  Maximum number of characters that can be in text string.
	int icolor;						//  Color of font when that was needed.  Still passed by functions, but probably not needed.
	int ibackcolor;					//  Background color, replaced by art, but still passed to functions.
	int iButtonWidth;				//  Width of OK and Cancel buttons.
	int iButtonHeight;				//  Height of OK and Cancel buttons.
	int iOKx;						//  Left edge of OK button.
	int iCancelx;					//  Left edge of Cancel button.
	int iButtonZ;					//  Top of buttons.  If -1 then Enter key is used to save and exit.
	int iFirstScreenChar;			//  First character of sFrontText to be drawn on screen.
	GrFontBuff  *font;				//  Font being used for text.
	GrBuff *SREBuff;				//  Buffer that stuff is drawn to.
} MBWorkingText;

typedef struct stMBOneSide {		//  This holds information about the various sides, though is probably going to change.
	char	sSideName[NAMESIZE];	//  Name of side.
	int		iIsActive;				//  Is this a slot that is being used.
	int		iFeelings[NUMSIDES];	//  Numbers that represent how a side feels about itself and others.
	int		iROE;					//  ROEs for each side.
} MBOneSide;

typedef struct stMBOneMessage {			//  This is the structure for one message/wav call that can be used by a mission.  Can be text message, sound call, or both.
	char	sMessage[MESSAGELENGTH];	//  Text string that will appear on screen.
	int		iIsActiveM;					//  Is text message valid.
	char	sSoundFile[FNAMESIZE];		//  Sound call that will take place.
	int		iIsActiveS;					//  Is sound call valid.
} MBOneMessage;

typedef struct stMBDeliveryMethod {			//  Holds information about one A2G delivery method.  Held so we don't have to keep reloading from a file.
	long	lDeliveryID;					//  ID which represents the delivery method
	char	sDeliveryText[MENUTEXTSIZE];	//  Text which lets the person editing know what the method is.
} MBDeliveryMethod;

typedef struct stMBGoalInfo {		//  Is the structure which holds the info for one goal.
	long lGoalRating;				//  Identifies how important a goal is 1= Primary, 2 = Secondary.
	long lGoalType;					//  Identifies type of goal (Must Protext, Must Destroy, ...)
	long lObjectID;					//  Unique ID of Object to be watched.
//	long lWayptID;
	long lValue;					//  Holds values used for goal type.
	long lTargetObjectID;			//  Used for things like, "Must get close to" goals.
	long lGoodMsgID;				//  Number of message used in briefing if goal achieved
	long lBadMsgID;					//  Number of message used in briefing if goal failed
} MBGoalInfo;

typedef struct stMBGoalText {		//  Structure for string used for goal success/failure messages.
	char sMessage[GOALMESSAGESIZE];	//  Message string.
} MBGoalText;

typedef struct stMBStaticObject {	//  Structure for plug-ins place in mission builder.
	long X;							//  Holds X position in feet from left edge of world.
	long Y;							//  Holds Y position in feet.
	long Z;							//  Holds Z position in feet from top of world.
	int iheading;					//  Holds plug-in orientation.  May or may not be used.
	int itype;						//  Tells what type of plug-in this is.
	int luniqueID;					//  Holds unique ID for plug-in.  ID will hold certain info about the plugin.
	int istate;						//  Not used.  I think is was for forcing a thing to be destroyed, or such, in a mission.
	int iSide;						//  Tells what side a plug-in belongs.  This may not be needed, instead Side/Country will be determined by plug-in type.
	int iflag1;						//  Variable that can be used for bit flags.
	int iRandomChance;				//  Gives chance object will appear 1-100.
	char sIDName[NAMESIZE];			//  Holds a name to help identify a plug-in in the mission builder.  Not used "in game".
} MBStaticObject;

typedef struct stMBStaticObjectMID1 {  //  Old version of stMBStaticObject.  See that for variable description.
	long X;
	long Y;
	long Z;
	int iheading;
	int itype;
	int luniqueID;
	int istate;
	int iSide;
	int iflag1;
	char sIDName[NAMESIZE];
} MBStaticObjectMID1;

typedef struct stMBLatLongInfo {	//  Stucture which holds one Lat/Long position (degs, mins, secs).
	long lLatitude;					//  Holds the Latitude in degs, mins and secs.
	long lLongitude;				//  Holds the Longitude in degs, mins and secs.
} MBLatLongInfo;

typedef struct stMBObjectEventInfo {	//  Struture holds the data for one object event (and maybe time event).
	long lObjectEventType;				//  Identifies what triggers event.
	long lObjectEventMod;				//  Helps define trigger event, like Side or Object ID needed for trigger.
	long lObjectEventDist;				//  Defines distance (in NM) when needed to define triggering event.
	long lObjectID;						//  Holds ID of object that is watched to trigger event.
	long lActionID;						//  Holds ID of action which event triggers.
	long lActionVal1;					//  Holds part of data needed by action.
	long lActionVal2;					//  Holds part of data needed by action
} MBObjectEventInfo;

typedef struct stMBTextStr {		//  Structure used by variables which hold text from files, so they don't have to be loaded each frame.
	char sTline[MENUTEXTSIZE];		//  Holds the text string.
} MBTextStr;



//  Globals  ****************************************************************************
#ifdef SREMBUILDERSTUFF
int iHotObj = -1;					//  Identifies object being edited
int iHotStatic = -1;				//  Identifies plug-in be edited
int iWptStatus = -1;				//  Used when adding a waypoint.  Tell such things as if this is the first waypoint of a new path.
int iHotWPPath = -1;				//  Identifies what path is being edited.
int iHotWpt = -1;					//  Identifies what waypoint is being edited.
int iHotAct = -1;					//  Identifies what action is being edited.
int	iNumAIObjs = 0;					//  Identifies total number of objects currently being used by mission.
int iNumStaticObjs = 0;				//  Identifies total number of plug-ins currently being used by mission.
int iNumWPPaths = 0;				//  Identifies total number of paths currently being used by mission.
int iNumWayPts = 0;					//  Identifies total number of waypoints currently being used by mission.
int iNumActions = 0;				//  Identifies total number of actions currently being used by mission.
int iMovingWpt = -1;				//  Identifies if and what waypoint is being moved.
int iMovingScen = -1;				//  Identifies if and what plug-in is being moved.
int iMovingWptStatus = 0;			//  Identifies things about a waypoint being moved, such as is it the first or last waypoint, is it the first alternate waypoint.
int iBuilderStatus = 0;				//  Used to identify that we are reading the name of a text file to read or write to.
long lLastMoveID = 0;				//  Used to hold a starting point for creating a new ID for a plane.
MBObject	AIObjects[MAXAIOBJS];	//  Holds the data about the objects in the mission.
long lHideMoveObjList[MAXAIOBJS];	//  Holds flags that are checked when displaying objects.
MBStaticObject AIStaticObjects[MAXSTATICOBJS];	//  Holds data about the plug-ins in the mission.
long lHideStaticObjList[MAXSTATICOBJS];	//  Holds flags that are checked when displaying plug-ins in the mission.
MBWPPaths AIWPPaths[MAXAIOBJS];		//  Holds data about the paths in the mission.
MBWayPoints AIWayPoints[MAXWPTS];	//  Holds data about the waypoints in the mission.
MBActions	AIActions[MAXACTS];		//  Holds data about the actions in the mission.
long lMBX0 = 0;						//  Holds the displayed left edge (in feet) from the edge of world.
long lMBZ0 = 0;						//  Holds the displayed top edge (in feet) from the edge of world.
long lMBX1 = 9999999;				//  Holds the displayed right edge (in feet) from the edge of world.
long lMBZ1 = 9999999;				//  Holds the displayed bottom edge cooridinate (in feet) from the edge of world.
long lMBMapStartX0;					//  Stores the displayed left edge cooridinate (in feet) from the edge of the world which the user wants shown when the mission is first loaded into the builder/briefing. 
long lMBMapStartZ0;					//  Stores the displayed top edge cooridinate (in feet) from the edge of the world which the user wants shown when the mission is first loaded into the builder/briefing.
long lMBMapStartX1;					//  Stores the displayed right edge cooridinate (in feet) from the edge of the world which the user wants shown when the mission is first loaded into the builder/briefing.
long lMBMapStartZ1;					//  Stores the displayed bottom edge cooridinate (in feet) from the edge of the world which the user wants shown when the mission is first loaded into the builder/briefing.
int  iMapScreenOrgX = MAPSCREENX0;	//  Holds the left edge of the map display area in screen coordinates.
int  iMapScreenOrgZ = MAPSCREENZ0;	//  Holds the top edge of the map display area in screen coordinates.
int  iMapScreenMaxX = MAPSCREENX;	// was 640	//  Holds the displayed map area width.
int  iMapScreenMaxZ = MAPSCREENZ;	// was 480	//  Holds the displayed map area height.
long lMapMaxX = 10000000;			//  Holds the width of the world (X), in feet, for the loaded world.
long lMapMaxZ = 10000000;			//  Holds the length of the world (Z), in feet, for the loaded world.
long lMapBoxX0 = 0;					//  When creating a dragged zoom box, this holds the screen X position where the mouse was first clicked to create the box.
long lMapBoxZ0 = 0;					//  When creating a dragged zoom box, this holds the screen Z position where the mouse was first clicked to create the box.
char MissionTitle[TITLETEXTLEN];	//  This is the text string that holds the title for the mission.
char MissionText[MAXTEXTLEN];	//  was 400	//  This is the text string that holds the long briefing for the mission.
char MissionSText[SHORTTEXTLEN];	//  This is the text string that holds the short briefing for the mission.
char TempStr[80];					//  This is a generic temporary text string.
MBMenu	MainMenu;					//  Holds the data for the Main Menu (top menu box) which is used for initial menu selection, and then to allow the user to jump back up to higher level (parent) menus, when he is in sub menus.
MBMenu  SubMenu;					//  This holds the main working menu which is used for mission editing control.  From here further sub menus, or editing functions will be called.
MBMenu  PlaneTypeMenu;				//  This holds the plane list menu.  This is referenced enough that it needs to be always loaded.
MBMenu  FuncMenu;					//  This is used when a menu is needed out side of the MainMenu/SubMenu constraint.
MBInstructText ActInstruct;			//  This holds the data for when we want to give instructions during an editing function.  Can be placed anywhere, but for now I place it where the SubMenu is.
long MenuCoreVals[20];  //  This holds the main value being worked on for each menu.  The edit plane menu, for example, will store the current plane number being worked on.
MBMenuFunc  MenuActFunc;			//  This is what is used when we want to do work that requires more than the MainMenu/SubMenu can do.  You can set it up to call functions to edit text, get integer valuse, or do anything you want (that can use up to 7 variable, and a return value).  You can use it for menu work as well, in conjunction with FuncMenu.
MBWorkingText WorkingText;			//  This is that data area that is used when doing generic text editing.
MBOneSide	Sides[NUMSIDES];		//  This holds the information about the different Sider/Countries in the mission.  This may change.
char sOKText[20];					//  This holds the "OK" text string so we don't have to keep doing file accesses.
char sCancelText[20];				//  This holds the "Cancel" text string so we don't have to keep doing file accesses. 
char sOnText[20];					//  This holds the "On" text string so we don't have to keep doing file accesses. 
char sOffText[20];					//  This holds the "Off" text string so we don't have to keep doing file accesses.
char sDistText[20];					//  This holds the "nm" text string so we don't have to keep doing file accesses.
int iIsMapZoomed;					//  Let's us know if the map is currently zoomed.
int iMapSlideX;						//  Used when using the horizontal map scroll bar to determine where new map location should be.
int iMapSlideZ;						//  Used when using the vertical map scroll bar to determine where new map location should be.
int iMapSlideWidth = 16;			//  Holds how wide (short dimension) the map scroll bar is when it is displayed.
DWORD dwMapTimer = 0;				//  Used as a delay when using scroll arrows on map slide bar.
char PlayerText[20];				//  This holds the "Player" text string so we don't have to keep doing file accesses.
char ComputerText[20];				//  This holds the "AI" text string so we don't have to keep doing file accesses.
char NetworkText[20];				//  This holds the "Net" text string so we don't have to keep doing file accesses.
MBOneMessage MessageList[MAXMESSAGES];	//  This holds the messages that the user want to use for the message.  He types in the text string he wants to see on the screen, and/or the name of the .wav file he wants to be played.
MBDeliveryMethod WeaponDelv[16];	//  This holds the information for picking a delivery method to use when bombing a target.
MBDeliveryMethod TempMenuTextHold[16];	//  This holds text we get from a file so we don't have to load it every time we want to display it during an editing function, but needing an ID number so we know what string to use (similar to menu).
MBGoalInfo GoalList[NUMGOALS];		//  This holds the information about the mission goals.
MBGoalText GoalText[NUMGOALS];		//  This holds the success and failure text briefing messages for mission goals.
long lMBWorldID = 1;				//  This holds the world ID of the last mission loaded, so we know if we have to reload/draw the terrain on screen when loading/creating a different mission.
int  iMissionVersionID;				//  This holds the mission ID for this mission which lets us know how it is to be loaded..
long lMissionCheckSum;				//  This holds the Mission check sum for this mission (not yet used).
int  iMissionCheckNum = MISSIONCHECKNUM;	//  This holds the value that says this is a mission, for this mission (when loading a mission it allows us to check to see if it is a real mission.
long lMissionTime = 0;				//  This holds the time when this mission is going to start.
int  iMissionWeather = 1;			//  This holds the weather for this mission
char sMissionFile[LONGFILENAME];	//  This holds the file name of this mission.
char sScenarioFile[LONGFILENAME];	//  This holds the name of a plug-in file.  Not needed for new missions, but needed for old.
char sScenarioPassword[PASSSIZE];	//  This holds the password (if any) needed to edit the scenario file.
int  iScenarioVersionID;			//  This holds the scenario ID which lets us know how this scenario file is to be loaded.
long lScenarioCheckSum;				//  This holds the check sum for this scenario file (not yet used).
int  iScenarioCheckNum = SCENARIOCHECKNUM;	//  This holds the value that lets us know we are loading a scenario file.
int  iShowRadarRings;				//  This lets us know if we are to draw radar rings on the map (not yet used).
int  iShowContourLines;				//  This lets us know if we are to show height information on the map (not yet used).
MBLatLongInfo lLLOrigin;			//  This holds the Lat/Long info for the upper left corner of the world.
MBLatLongInfo lLOrgLMax;			//  This holds the Lat/Long info for the upper right corner of the world.
MBLatLongInfo lLMaxLOrg;			//  This holds the Lat/Long info for the lower left corner of the world.
MBLatLongInfo lLMaxLMax;			//  This holds the Lat/Long info for the lower right corner of the world.
MBObjectEventInfo ObjEventList[NUMOBJEVENTS];	//  This holds the info about the object (and possibly time) events that can happen in the mission.
char sWorldStr[80];					//  This string holds the name of the world file which is to be loaded (from regit).
char sWorldPath[160];				//  This string holds the path of where the world file is loaded (from regit).
DWORD  dwMBRedrawTimer;				//  Used as a delay when moving map or changing its size so that the user has time to get where he wants to before it tries to redraw (which can be slow at larger zooms).
long lMBRDX0;						//  This holds the left edge coordinate of the displayed map when moving/zooming the map, before it is redrawn (so original transitional size can be represented).
long lMBRDZ0;						//  This holds the top edge coordinate of the displayed map	when moving/zooming the map, before it is redrawn (so original transitional size can be represented).
long lMBRDX1;						//  This holds the right edge coordinate of the displayed map when moving/zooming the map, before it is redrawn (so original transitional size can be represented).
long lMBRDZ1;						//  This holds the bottom edge coordinate of the displayed map when moving/zooming the map, before it is redrawn (so original transitional size can be represented).
int  iRDMapScreenMaxX;  // 640		//  This holds the displayed map area width, in screen coordinates, when moving/zooming the map.  This is so we know if the scroll bars were up before modifying the map so original trasitional size can be represented.
int  iRDMapScreenMaxZ;  // 480		//  This holds the displayed map area width, in screen coordinates, when moving/zooming the map.  This is so we know if the scroll bars were up before modifying the map so original trasitional size can be represented.
int  iMBTempRedraw;					//  This lefts us know if we are to do a partial redrawing of the screen (not redrawing terrain).
long lMBMainShow = 1;				//  This holds bit flags that let us know about various general map states (Lat/Long or map option box can be redrawn, information about the ball in the corner actions, etc.).
char sMBNone[20];					//  This holds the "None" text string so we don't have to keep doing file accesses.
char sMBGallon[20];					//  This holds the "gals" text string so we don't have to keep doing file accesses.
MBTextStr TextHold[30];				//  This holds text we get from a file so we don't have to load it every time we want to display it during an editing function (like the headers for goals).
HCURSOR hMBWaitCursor;				//  Holds the information so we can use the wait cursor.
HCURSOR hMBOrgCursor;				//  Holds the information so we can use the orginal cursor (after using wait cursor).
extern GrBuff *GrBAcceptReg;
extern GrBuff *GrBAcceptHot;
extern GrBuff *GrBAcceptSel;
extern GrBuff *GrBCancelReg;
extern GrBuff *GrBCancelHot;
extern GrBuff *GrBCancelSel;
extern GrBuff *GrBHelpReg;
extern GrBuff *GrBHelpHot;
extern GrBuff *GrBHelpSel;


#else
extern int iHotObj;
extern int iHotStatic;
extern int iWptStatus;
extern int iHotWPPath;
extern int iHotWpt;
extern int iHotAct;
extern int iNumAIObjs;
extern int iNumStaticObjs;
extern int iNumWPPaths;
extern int iNumWayPts;
extern int iNumActions;
extern int iMovingWpt;
extern int iMovingScen;
extern int iMovingWptStatus;
extern int iBuilderStatus;
extern long lLastMoveID;
extern MBObject	AIObjects[MAXAIOBJS];
extern long lHideMoveObjList[MAXAIOBJS];
extern MBStaticObject AIStaticObjects[MAXSTATICOBJS];
extern long lHideStaticObjList[MAXSTATICOBJS];
extern MBWPPaths AIWPPaths[MAXAIOBJS];
extern MBWayPoints AIWayPoints[MAXWPTS];
extern MBActions	AIActions[MAXACTS];
extern long lMBX0;
extern long lMBZ0;
extern long lMBX1;
extern long lMBZ1;
extern long lMBMapStartX0;
extern long lMBMapStartZ0;
extern long lMBMapStartX1;
extern long lMBMapStartZ1;
extern int  iMapScreenOrgX;
extern int  iMapScreenOrgZ;
extern int  iMapScreenMaxX;
extern int  iMapScreenMaxZ;
extern long lMapMaxX;
extern long lMapMaxZ;
extern long lMapBoxX0;
extern long lMapBoxZ0;
extern char MissionTitle[TITLETEXTLEN];
extern char MissionText[MAXTEXTLEN];
extern char MissionSText[SHORTTEXTLEN];
extern char TempStr[80];
extern MBMenu	MainMenu;
extern MBMenu  SubMenu;
extern MBMenu  PlaneTypeMenu;
extern MBMenu  FuncMenu;
extern MBInstructText ActInstruct;
extern long MenuCoreVals[20];  //  This holds the main value being worked on for each menu.  The edit plane menu, for example, will store the current plane number being worked on.
extern MBMenuFunc  MenuActFunc;
extern MBWorkingText WorkingText;
extern MBOneSide	Sides[NUMSIDES];
extern char sOKText[20];
extern char sCancelText[20];
extern char sOnText[20];
extern char sOffText[20];
extern char sDistText[20];
extern int iIsMapZoomed;
extern int iMapSlideX;
extern int iMapSlideZ;
extern int iMapSlideWidth;
extern DWORD dwMapTimer;
extern char PlayerText[20];
extern char ComputerText[20];
extern char NetworkText[20];
extern MBOneMessage MessageList[MAXMESSAGES];
extern MBDeliveryMethod WeaponDelv[16];
extern MBDeliveryMethod TempMenuTextHold[16];
extern MBGoalInfo GoalList[NUMGOALS];
extern MBGoalText GoalText[NUMGOALS];
extern long lMBWorldID;
extern int  iMissionVersionID;
extern long lMissionCheckSum;
extern int  iMissionCheckNum;
extern long  lMissionTime;
extern int  iMissionWeather;
extern char sMissionFile[LONGFILENAME];
extern char sScenarioFile[LONGFILENAME];
extern char sScenarioPassword[PASSSIZE];
extern int  iScenarioVersionID;
extern long lScenarioCheckSum;
extern int  iScenarioCheckNum;
extern int  iShowRadarRings;
extern int  iShowContourLines;
extern MBLatLongInfo lLLOrigin;
extern MBLatLongInfo lLOrgLMax;
extern MBLatLongInfo lLMaxLOrg;
extern MBLatLongInfo lLMaxLMax;
extern MBObjectEventInfo ObjEventList[NUMOBJEVENTS];
extern char sWorldStr[80];
extern char sWorldPath[160];
extern DWORD  dwMBRedrawTimer;
extern long lMBRDX0;
extern long lMBRDZ0;
extern long lMBRDX1;
extern long lMBRDZ1;
extern int  iRDMapScreenMaxX;  // 640
extern int  iRDMapScreenMaxZ;  // 480
extern int  iMBTempRedraw;
extern long lMBMainShow;
extern MBTextStr TextHold[30];
extern HCURSOR hMBWaitCursor;
extern HCURSOR hMBOrgCursor;
extern char sMBNone[20];
extern char sMBGallon[20];
extern GrBuff *GrBAcceptReg;
extern GrBuff *GrBAcceptHot;
extern GrBuff *GrBAcceptSel;
extern GrBuff *GrBCancelReg;
extern GrBuff *GrBCancelHot;
extern GrBuff *GrBCancelSel;
extern GrBuff *GrBHelpReg;
extern GrBuff *GrBHelpHot;
extern GrBuff *GrBHelpSel;


#endif

void MBInitGrBuffs();
void MBFreeGrBuffs();
void MBDoGeneralCancel();
void DefaultInitWayPoints();
void InitWayPoints();
//void DrawNewWayPoint(GrBuff *SREBuff, long lWPx, long lWPz);
void DrawNewWayPoint(GrBuff *SREBuff, long lWPx, long lWPz, int MoveStatus);
void DrawWptsWOutMove(GrBuff *SREBuff, int movewpt);
void DrawWptsWOutAdd(GrBuff *SREBuff, int addwpt);
//void DrawMoveWayPoint(GrBuff *SREBuff, long lWPx, long lWPz, int movewpt);
void DrawMoveWayPoint(GrBuff *SREBuff, long lWPx, long lWPz, int movewpt, int movestatus);
void DrawWayPoints(GrBuff *SREBuff);
void MBGet8to16Color(int color8, int *rval, int *gval, int *bval);
void MBGetIconMod(int wpx, int wpz, int *xmod, int *zmod, int *wmod, int *hmod, int iconheight, int iconwidth, int orgx, int orgz, int maxx, int maxz);
void MBReDrawTopIcons();
void MBDrawAWayPointShape(GrBuff *SREBuff, int waynum, int startpt, int pathnum);
void DrawWayShape(GrBuff *SREBuff, long lWPx, long lWPz, int waycolor, int waynum);
void DrawStartWayShape(GrBuff *SREBuff, long lWPx, long lWPz, int waycolor, int waynum, int objnum);
void DrawStartPlayerWayShape(GrBuff *SREBuff, long lWPx, long lWPz, int waycolor);
void DrawActiveWayShape(GrBuff *SREBuff, long lWPx, long lWPz);
void DrawStartAIWayShape(GrBuff *SREBuff, long lWPx, long lWPz, int waycolor);
void DrawStartNetWayShape(GrBuff *SREBuff, long lWPx, long lWPz, int waycolor);
void DrawStaticObjShape(GrBuff *SREBuff, long lWPx, long lWPz, int objnum, int waycolor);
//void AddWayPoint(long lWPx, long lWPz);
void AddWayPoint(long lWPx, long lWPz, int iNewWpt);
void UpdateWayPoint(int movewpt, long lWPx, long lWPz);
void UpdateScenario(int moveobj, long lWPx, long lWPz);
int GetWptAt(long lWPx, long lWPz);
int GetScenarioAt(long lWPx, long lWPz);
int GetMovingWptStatus(int MoveWpt);
void DeleteWayPoint(int iOldWpt);
void DeleteWaypoints(int startwp, int numwp);
void CheckInvalidObjects(void);
void MBScreenToWorldConvert(int startx, int startz, long *endx, long *endz);
void MBWorldToScreenConvert(long startx, long startz, int *endx, int *endz);
void DrawWayPtLine(GrBuff *SREBuff, long lWPx1, long lWPz1, long lWPx2, long lWPz2, int waytype);
void MBZoomMap(void);
void MBUnZoomMap(void);
void MBMapLeft(void);
void MBMapRight(void);
void MBMapUp(void);
void MBMapDown(void);
void MBCenterOnWP(int centerwp);
void MBCenterAt(long centx, long centz);
void SetNewMapZoom(long boxx0, long boxz0, long boxx1, long boxz1);
void AddGroupPlanes(int leadplane, int numadd);
void CheckAddGroupPlaneEffects(int leadplane, int numadd);
void RemoveGroupPlanes(int leadplane, int numremove);
void CheckRemoveGroupPlaneEffects(int leadplane, int numremove);
void AIRemovePlanePaths(int planenum);
void AIRemovePath(int pathnum);
void MBFinishRemovingPath(int pathnum);
void RemoveScenarioObj(int objectnum);
void MBSetUpMissionWorld(long worldid, int lastworldid);
int ReadMissionData(char *file_name);
int MBReadOldMissionID1(int MissionFile);
void MBConvertFromID1Obj(MBObject *newobj, MBObjectMID1 oneoldobj, int *numingroup, int cnt);
int ReadScenarioData(char *file_name);
int ReadOldScenarioDataID1(int MissionFile);
void MBConvertFromID1Scenario(MBStaticObject *newstatic, MBStaticObjectMID1 oneoldstatic);
void CheckMissionData();
int WriteMissionData(char *file_name);
int WriteScenarioData(char *file_name);
int ProcessTextChar(char newchar, char *workstr, int maxlen, int actlen);
int ProcessNumChar(char newchar, char *workstr, int maxlen, int actlen);
void AddNewObjectGroup(int startwp, int numwps);
void SwitchControlType(int planenum);
int GetWayPtObj(int wptnum);
int GetWayPtPath(int wptnum);
void MBDrawBoxFillABS(GrBuff *SREBuff, long MinX, long MinZ, long MaxX, long MaxZ, int bcolor);
void MBDrawBoxFillRel(GrBuff *SREBuff, long MinX, long MinZ, long bwidth, long bheight, int bcolor);
long GetStringPixelLength(char *wstr, GrFontBuff *font);
int WriteStringInBox(char *wstr, GrBuff *SREBuff, GrFontBuff *font, int MinX, int MinZ, int width, int height, int color, int newlinespace, long *xstart, int center);
int HeightOfStringInBox(char *wstr, GrFontBuff *font, int width, int height, int newlinespace);
int GetMaxFontHeight(GrFontBuff *font);
int GetMaxFontWidth(GrFontBuff *font);
int LoadMenuFile(char *menufile, MBMenu *newmenu);
void ShowMenu(MBMenu *menu, GrFontBuff *font, GrFontBuff *hotfont, GrFontBuff *coldfont, GrBuff *SREBuff, int color, int hotcolor, int backcolor, int mx, int mz, int currbtn, int lastbtn);
//void ShowMenu(MBMenu *menu, GrFontBuff *font, GrBuff *SREBuff, int color, int hotcolor, int backcolor, int mx, int mz, int currbtn, int lastbtn);
int MBMenuPageUp(MBMenu *menu, GrFontBuff *font, int bottom);
int CheckSlidePosition(int topslidebox, int minedge, int maxedge, int numitems);
int MBProcessMenuItem(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBCheckForMenuAction(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBObjectMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBPlaneMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int RandomMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBWayPtMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBOrdinanceMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBRoeMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBSideRoeMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBScenarioMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBEScenarioMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBEScenarioObjMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBMapShowMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBHSClassMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBAddPlaneSetup(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBAddVehicleSetup(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBDoZoomBox(GrBuff *SREBuff, int xval, int yval, int LMB, GrBuff *SREBuff2, GrBuff *MapBuff);
void MBDoZoomBoxC(GrBuff *SREBuff, int xval, int yval, int LMB, GrBuff *SREBuff2, GrBuff *MapBuff);
void MBHandleNewWPs(GrBuff *SREBuff, GrBuff *SREBuff2, GrBuff *MapBuff, int xval, int yval, long worldxval, long worldyval, int LMB, int LastMouseLB);
void MBGetMenuLine(int menuid, int itemid, char *orgline, char *returnline);
void MBPlaneMenuLine(int itemid, char *orgline, char *returnline);
void MBRandomMenuLine(int itemid, char *orgline, char *returnline);
void MBScenarioMenuLine(int itemid, char *orgline, char *returnline);
void MBEScenObjMenuLine(int itemid, char *orgline, char *returnline);
void MBOrdinanceMenuLine(int itemid, char *orgline, char *returnline);
void MBMissionMenuLine(int itemid, char *orgline, char *returnline);
void MBMapShowMenuLine(int itemid, char *orgline, char *returnline);
void MBAddPlaneTypeText(int ptype, char *orgline, char *returnline);
void MBAddVehicleTypeText(int ptype, char *orgline, char *returnline);
void TestFunction();

int MBInstructSetup(MBMenu *menu, GrFontBuff *font, int color, int backcolor, char *filename);
void MBGetPlanesAt();
void MBGetVehiclesAt();
void MBGetMovingObjsAt();
void MBGetScenarioObjsAt();
void MBGetAllAt();
void MBGetObjectsAt(int doplanes, int dovehicles, int doother);
void MBProcessActFunc(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBObjectActFunc();
void MBEScenarioActFunc();
//void MBEScenObjActFunc();
void MBMapShowActFunc();
void MBDoFuncMenu();
//int DoGetText(MBWorkingText *WorkingText, short mousex, short mousey);
int DoGetText(MBWorkingText *WorkingText, short mousex, short mousey, int drawframe);
//void InitMissionText(MBWorkingText *WorkingText, MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void InitMissionText(MBWorkingText *WorkingText, MBMenu *menu, GrFontBuff *font, int color, int backcolor, int MinX, int MinZ, int width, int height, int dobuttons, int maxchars, int isslide);
int InitGetText(MBWorkingText *WorkingText, char *inputfile);
void MBSetNewTextCursorLoc(MBWorkingText *WorkingText,int inbacktext, char *wptr);
void MBCheckTextEndOfLine(MBWorkingText *WorkingText, char *placeptr, int inbacktext);
void MBGetTextCursorLoc(MBWorkingText *WorkingText, short mousex, short mousey, int width, int height);
void MBTextPageDown(MBWorkingText *WorkingText, int width, int height);
void MBTextPageUp(MBWorkingText *WorkingText, int width, int height);
void MBTextLineDown(MBWorkingText *WorkingText, int width, int height, int updatecursor);
void MBTextLineUp(MBWorkingText *WorkingText, int width, int height);
int WriteTextInBox(MBWorkingText *WorkingText, int width, int height);
void ReadMBKeyboard(void);
void MBReDrawTempMap();
void MBOldWorldToScreenConvert(long startx, long startz, int *endx, int *endz);
void MBGetVehicleIDName(int planenum, char *sptr);
void MBGetStaticIDName(int staticobjnum, char *sptr);
void MBDrawLatLongInfo(long worldxval, long worldyval);
void MBDrawMapOptions(long mousex, long mousey, int wpstatus, int menufuncnull, short mouselb, short lastlb);
void MBAddSubMenu();
void MBRemoveSubMenu();
void MBInitInitialText();
void MBReSetAllRedraws();


//  mbuild2.cpp ******************************************************************
int MBDoTextSlide(MBWorkingText *WorkingText, int height, int width, short mousex, short mousey, int arrowwidth);
void MBGetTextSlideInfo(MBWorkingText *WorkingText, int width, int height, int *numlines, int *fline, int *numshown);
void MBGetPlaneIDName();
void MBGetStaticIDName();
void InitPlaneNameText(MBWorkingText *WorkingText, MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBInitSideMenu(long objectclass);
void InitOpponents();
int TextStringInBox(char *wstr, GrBuff *SREBuff, GrFontBuff *font, int MinX, int MinZ, int width, int height, int color, int newlinespace, long *xstart, int center);
int MBSideMenu();
void MBEditSideInfo();
void MBDrawStringVert(GrBuff *dest_buff, GrFontBuff *font, int x, int y, char *szCharString, int color, int ybounds);
void InitSideInfoText(MBWorkingText *WorkingText, int MinX, int MinZ, int width, int height, GrFontBuff *font, int color, int backcolor, char *wstr, int isnum);
void MBGetSideInfo();
int MBShowSideInfo();
int MBShowMapSlide(GrBuff *SREBuff, long mousex, long mousey, int allowslide, short mbutton, short lastmbutton);
int MBCopyMapSlide(GrBuff *SREBuff, long mousex, long mousey, int allowslide, short mbutton, short lastmbutton);
void MBSetNewMapLoc(int newx, int newz);
void MBAddPlaneControlText(int ptype, char *orgline, char *returnline);
int MBGetPlaneNumSetup(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBInitGetPlaneNumText(MBWorkingText *WorkingText, int MinX, int MinZ, int width, int height, GrFontBuff *font, int color, int backcolor, char *wstr);
void MBGetPlaneNum();
void MBGetWayPtAt(int updatemenucore);
void MBCheckScenarioAt();
int MBUpdateHotWpt(int *planenum);
int MBAddWayPtSetup(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBGetNextWP(int planenum, int waypt);
int MBGetRelWPNum(int planenum, int wayptnum);
void MBWayPtMenuLine(int itemid, char *orgline, char *returnline);
int AddAction(int iNewAct, int numacts, int actid, int nowpactions);
int MBActionMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBGetActionSize(int actionid);
int MBGetSpecificActionSize(int actionnum);
void MBMenuModify(MBMenu *menu);
void MBCheckForExistingActions(MBMenu *menu);
void MBCheckActionMenuValid(MBMenu *menu);
void MBDeleteAction(int iOldAct);
void MBDeleteAnActionElement(int iOldAct);
void CheckRemoveActionEffects(int actionnum, int numremove);
int MBInitActionMenu();
void MBActionActFunc(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBCheckMissionMenuValid(MBMenu *menu);
void MBCheckObjectMenuValid(MBMenu *menu);
void MBCheckPlaneMenuValid(MBMenu *menu);
void MBCheckPlaneControlMenuValid(MBMenu *menu);
void MBCheckWPMenuValid(MBMenu *menu);
void MBCheckOdinanceMenuValid(MBMenu *menu);
void MBCheckGoalMenuValid(MBMenu *menu);
void MBCheckObjEventMenuValid(MBMenu *menu);
void MBSetUpGetNum(int maxval1, int maxval2, int numtype, int val1, int val2, MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetActNumVal();
void MBSetupEditAction(long actid, MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int ConvertHeadingFI(long heading);
int ConvertSpeedFI(long speed);
int GetMaxFontWidthNum(GrFontBuff *font);
int MBNumInc(int chnchar, int slen, int actval, int maxval);
int MBNumDec(int chnchar, int slen, int actval, int maxval);
int ConvertTOTFI(long time, int *minutes);
long ConvertHeadingIF(int heading);
long ConvertSpeedIF(int speed);
long ConvertTOTIF(int hours, int minutes);
void MBUpdateActionVal(long actnum, int actid, int val1, int val2);
void MBActionMenuLine(int itemid, long itemval, char *orgline, char *returnline);
void MBGetSepStr(int sepid, char *retstr);
void MBActionValFunc(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetWayPtAtNV();
void MBReleasePlane();
int MBCheckForCoverPlanes();
int MBShowLinkedPaths(GrBuff *SREBuff, long actionval, long *lastx, long *lastz, int linkplane, int drawfirst);
void DrawWayPtLineOff(GrBuff *SREBuff, long lWPx1, long lWPz1, long lWPx2, long lWPz2, int waytype, int offx, int offz);
int MBGetLinkedPathStartXZ(long actionval, long *lastx, long *lastz);
int MBGetLinkedPathXZ(long actionval, long *lastx, long *lastz, int linkplane);
void MBDisplayErrorMessage();
void MBInitMessages();
void MBInitMessageMenu();
int MBMessageMenu();
void MBEditMessageInfo();
void InitMessageInfoText(MBWorkingText *WorkingText, int MinX, int MinZ, int width, int height, GrFontBuff *font, int color, int backcolor, char *wstr, int isnum);
void MBGetMessageInfo();
int MBShowMessageInfo();
long MBGetNumBombing(int planenum, int waypt);
long MBCheckNumBombActions(int firstact, int numbombing, int planenum, int waypt);
int MBShowDetailedBomb(GrFontBuff *font, int color, int backcolor, long firstact, int checkmouse, int noshowid, int noshowval);
void MBSetupBombing(MBMenu *menu, GrFontBuff *font, int color, int backcolor, long firstact);
void MBEditBombing();
void MBBombSetUpIngress(int changeid);
void MBGetIngress();
void MBBombSetUpTarget();
int  MBDTargetMenu();
void MBBombSetUpDelivery();
int MBDDeliveryMenu();
void MBBombSetUpBombTOT(int changeid);
void MBGetBombTOT();
void MBBombSetUpEgress(int changeid);
void MBGetEgress();
long MBGetActNumValRet(int showret);
void MBDoFuncMenuPlus();
long MBGetNumBombingGroup(int planenum, int waypt, int searchval, int curval, int *numgroup);
void MBSetupGoals(MBMenu *menu, GrFontBuff *font, int color, int backcolor, long firstact);
void MBEditGoals();
int MBShowGoals(GrFontBuff *font, int color, int backcolor, long goalnum, int checkmouse, int noshowgoal, int noshowitem);
void MBGetObjectInfoText(long objectnum, long waynum, char *returntext);
void MBGetStaticObjectInfoText(long objectnum, char *returntext);
void MBInitGoals();
void MBGoalSetUpRating(int changeid);
void MBGetGoalRating();
void MBGoalSetUpType();
int MBDGoalTypeMenu();
void MBGoalSetUpObject();
int MBDGoalObjectMenu();
void MBGoalSetUpWaypt();
int MBDGoalWayptMenu();
void MBGoalSetUpNum(int changeid);
void MBGetGoalNum();
void MBGoalSetUpCloseObject();
int MBDGoalCloseObjectMenu();
void MBGetGoalHowCloseNum();
void MBGoalSetUpMessage(int changeid, int messageid, int MinX, int MinZ, int width);
void MBGetGoalMessageInfo();
void InitGoalMessageText(MBWorkingText *WorkingText, MBMenu *menu, GrFontBuff *font, int color, int backcolor, int MinX, int MinZ, int width);
void MBShowGoalSlideMenu(GrBuff *SREBuff, int color, int backcolor, int mx, int mz, int currbtn, int lastbtn, int MinA, int MinZ, int height, int numlines, int shownlines, int incval, int doextra);
int MBMissionMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetBriefing();
void MBGetShortBriefing();
int MBMissionWorldMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBWeatherMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBSetUpFileNames(int fileaction, int domore);
int MBFileMissionMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBLoadFileMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
int MBSaveFileMenu(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void InitGeneralInfoText(MBWorkingText *WorkingText, int MinX, int MinZ, int width, int height, GrFontBuff *font, int color, int backcolor, char *wstr, int isnum, int maxsize, int textfront);
int MBGetGeneralInfo(int retval);
void GetSaveFileName();
void MBOrderMenu(MBMenu *menu, int numinarea, int isload);
void MBRandomSetUpChance(int planenum, MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetRandomChance();
int MBGetMaxChance(int planenum);
void MBRandomSetUpGroup(int planenum, MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetRandomGroup();
int MBGetMaxGroup(int planenum);
void MBRandomSetUpElement(int planenum, MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetRandomElement();
int MBGetMaxElement(int planenum);
int MBShowRandom(GrFontBuff *font, int color, int backcolor, int planenum, int groupnum, int elementnum, int showret);
void MBShowGeneralSlideZ(GrBuff *SREBuff, int color, int backcolor, int mx, int mz, int currbtn, int lastbtn, int MinA, int MinZ, int height, int numlines, int shownlines);
void MBShowGeneralSlideX(GrBuff *SREBuff, int color, int backcolor, int mx, int mz, int currbtn, int lastbtn, int MinA, int MinZ, int width, int numlines, int shownlines);
void ShowRandomGroups();
void GetAltitudes();
int EditAltitudes();
void MBSetUpAltText();
long MBGetDistNM(long dist, long *hundreths);
long MBGetTotalWPDist(int objectnum, int startwp, int startcnt, int linkedobj, long *retx, long *retz);
long MBGetWPDist(int waypt1, int waypt2, int objectnum);
long MBGetDistWPNumGroup(int objectnum, int startwp, int startcnt, int linkedobj, long *retx, long *retz, int lookfor, int *wayptnum);
long MBGetDistWPNum(int waypt1, int waypt2, int lookfor, int *wayptnum, int objectnum);
long MBGetTotalWPCntLinked(int objectnum, int startwp, int startcnt, int linkedobj);
long MBWPCntLinked(int waypt1, int waypt2, int objectnum);
long MBGetHeightWPNumGroup(int objectnum, int startwp, int startcnt, int linkedobj, int lookfor, int *wayptnum, int *isroot);
long MBGetHeightWPNum(int waypt1, int lookfor, int *wayptnum, int objectnum, int *isroot);
void MBAltSetUpSingle(int wayptnum);
void MBGetAltSingle();
int MBGetMaxFuel(int planenum);
void MBFuelSetUp(int planenum, MBMenu *menu);
void MBGetFuel();
long MBGetWPDistNew(int waypt1, long lWPx2, long lWPz2, int objectnum);
void MBShowWayDistNew(int waypt1, long lWPx, long lWPz, int objectnum);
void MBGetWeaponText(int weaponid, char *returnstr);
void MBInitWeaponMenu();
void MBWeaponMenu();
int MBGetMaxWeapon(int objecttype, int weapontype);
void MBNumWeaponSetUp(int planenum, MBMenu *menu);
void MBGetNumWeapons();
void MBGetWeatherText(int weatherid, char *returnstr);
void MBMissionTimeSetUp(MBMenu *menu);
void MBGetMissionTime();
void MBInitSideROEMenu(MBMenu *menu);
void UpOneMainMenuLevel();
void MBGetTitle();
void MBGetVehicleText(int vehicleid, char *returnstr);
void MBInitVehicleMenu();
void MBSetUpCheckSPassword(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetScenarioPassword();
void MBSetUpNewSPassword(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetNewScenarioPassword();
long MBGetNextMovingID();
int MBGetVehicleFromID(long lookforID);
long MBGetNextScenarioID();
int MBGetScenarioObjFromID(long lookforID);
void AddNewStaticObject(long worldx, long worldz);
void MBHandleNewStatic();
void MBGetIntNumSetUp(long orgval, long maxval, int *changingvar, MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetLongNumSetUp(long orgval, long maxval, long *changingvar, MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBGetLongNum();
void MBGetStaticText(int staticid, char *returnstr);
void MBInitStaticMenu();
void MBGetInfoTextOffID(long IDnum, int waynum, char *returnstr);
void MBVariousMenuText(int itemid, char *smenufile, char *returnstr);
int MBGetHidenObjects();
void MBInitSideShowMenu();
void MBShowSideUpdate(int sideid, int hideside);
long MBConvertLatLongToL(int degrees, int minutes, int seconds);
int MBConvertLToLatLong(long val, int *minutes, int *seconds);
void MBGetCurrLatLong(long mapx, long mapz, int *latdeg, int *latmin, int *latsec, int *longdeg, int *longmin, int *longsec);
void MBLL2MinusLL1(int lld2, int llm2, int lls2, int lld1, int llm1, int lls1, int *dd, int *dm, int *ds);
void MBPercLL(float fperc, int degval, int minval, int secval, int *retdeg, int *retmin, int *retsec);
void MBGetCurrLorL(int dolat, float fworkval, float fworkval2, long lLLo, long lLoLm, long lLmLo, long lLmLm, int *ldeg, int *lmin, int *lsec);
void MBCheckLL(int *ldeg, int *lmin, int *lsec);
void MBSetupObjectEvents(MBMenu *menu, GrFontBuff *font, int color, int backcolor);
void MBEditObjectEvent();
int MBShowObjectEvents(GrFontBuff *font, int color, int backcolor, int checkmouse, int noshowevent, int noshowitem);
void MBShowObjEventSlideMenu(GrBuff *SREBuff, int color, int backcolor, int mx, int mz, int currbtn, int lastbtn, int MinA, int MinZ, int height, int numlines, int shownlines, int incval, int doextra);
void MBInitObjectEvents();
void MBObjEventSetUpType();
int MBDObjEventTypeMenu();
void MBObjEventSetUpObject();
int MBDObjEventObjectMenu();
int MBDObjEventModMenu();
void MBObjEventSetUpAction();
int MBDObjEventActionMenu();
void MBInitObjEventSideMenu();
int MBDObjEventSideMenu();
int MBObjEventMessageMenu();
int MBDObjEventCloseObjectDist();
void MBGetObjEventHowCloseNum();
void MBGrCopyRectGradZ(GrBuff *dest,int Dx,int Dy, GrBuff *source, int Sx,int Sy,int Sw,int Sh, int MaxSh);
void MBGrCopyRectGradX(GrBuff *dest,int Dx,int Dy, GrBuff *source, int Sx,int Sy,int Sw,int Sh, int MaxSw);
void MBGrDrawLineClipped8to16(GrBuff *SREBuff, int X0, int Z0, int X1, int Z1, int colorval8);
void MBGrCopyRectGradZMask(GrBuff *dest,int Dx,int Dy, GrBuff *source, int Sx,int Sy,int Sw,int Sh, int MaxSh);
void MBGrCopyRectGradXMask(GrBuff *dest,int Dx,int Dy, GrBuff *source, int Sx,int Sy,int Sw,int Sh, int MaxSw);
void MBSetUpTextHold(char *filename);
int MBCheckForRandomGroups(int morethanone);
int MBCheckBallSelection(long MouseX, long MouseY,int iMode, GrBuff *SREBuff);
int MBCheckOKButton(short mouselb, short lastlb, int mousex, int mousey, int buttonx, int buttonz, int buttonwidth, int buttonheight, GrBuff *SREBuff);
int MBCheckCancelButton(short mouselb, short lastlb, int mousex, int mousey, int buttonx, int buttonz, int buttonwidth, int buttonheight, GrBuff *SREBuff);
int MBSetUpAlternatePath();


//*  Temp font prototype for now  eventually put MBDrawStringVert with GrFont Code
void NewGrDrawChar(GrBuff *dest_buff,GrFontBuff *font, int x, int y, unsigned char ch, int color,unsigned char *Buffer);


#if 0
int InitWaypoints(void);
void DoAIs(void);
void FlyToWayPoint(Plane *planepnt);
void AIChangeDir(Plane *planepnt, double offangle, double dy);
#endif
