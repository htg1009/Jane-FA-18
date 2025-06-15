// Generated Translations File
// selson 12:20:20 AM  10/12/99
//
// SIMULATION
//
// AI COMMANDS
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: AIC_ENGAGE_BANDITS
//
// MessageText:
//
//  Engage Bandits%0
//
#define AIC_ENGAGE_BANDITS               0x00000001L

//  
//
// MessageId: AIC_RADAR_ON_OFF
//
// MessageText:
//
//  Radar On/Off%0
//
#define AIC_RADAR_ON_OFF                 0x00000002L

//  
//
// MessageId: AIC_MUSIC_ON_OFF
//
// MessageText:
//
//  Music On/Off%0
//
#define AIC_MUSIC_ON_OFF                 0x00000003L

//  
//
// MessageId: AIC_REPORT_CONTACTS
//
// MessageText:
//
//  Report Contacts%0
//
#define AIC_REPORT_CONTACTS              0x00000004L

//  
//
// MessageId: AIC_SORT_BANDITS
//
// MessageText:
//
//  Sort Bandits%0
//
#define AIC_SORT_BANDITS                 0x00000005L

//  
//
// MessageId: AIC_GROUND_MENU
//
// MessageText:
//
//  Ground Attack (Sub-Menu)%0
//
#define AIC_GROUND_MENU                  0x00000006L

//  
//
// MessageId: AIC_STATUS_MENU
//
// MessageText:
//
//  Status (Sub-Menu)%0
//
#define AIC_STATUS_MENU                  0x00000007L

//  
//
// MessageId: AIC_FORMATION_MENU
//
// MessageText:
//
//  Change Formation (Sub-Menu)%0
//
#define AIC_FORMATION_MENU               0x00000008L

//  
//
// MessageId: AIC_RETURN_TO_BASE
//
// MessageText:
//
//  Return To Base%0
//
#define AIC_RETURN_TO_BASE               0x00000009L

//  
//
// MessageId: AIC_SANITIZE_RIGHT
//
// MessageText:
//
//  Sanitize Right%0
//
#define AIC_SANITIZE_RIGHT               0x0000000AL

//  
//
// MessageId: AIC_SANITIZE_LEFT
//
// MessageText:
//
//  Sanitize Left%0
//
#define AIC_SANITIZE_LEFT                0x0000000BL

//  
//
// MessageId: AIC_COVER_REJOIN
//
// MessageText:
//
//  Cover%0
//
#define AIC_COVER_REJOIN                 0x0000000CL

//
// MessageId: AIC_COVER_ME
//
// MessageText:
//
//  Cover me.%0
//
#define AIC_COVER_ME                     0x0000000DL

//  
//
// MessageId: AIC_BEGIN_CAP
//
// MessageText:
//
//  Begin CAP%0
//
#define AIC_BEGIN_CAP                    0x0000000EL

//  
//
// MessageId: AIC_CONTINUE_ESCORT
//
// MessageText:
//
//  Continue Escort%0
//
#define AIC_CONTINUE_ESCORT              0x0000000FL

//  
//
// MessageId: AIC_ORBIT_REJOIN
//
// MessageText:
//
//  Orbit Here%0
//
#define AIC_ORBIT_REJOIN                 0x00000010L

//  
//
// MessageId: AIC_INTERCEPT_MENU
//
// MessageText:
//
//  Intercept (Sub-Menu)%0
//
#define AIC_INTERCEPT_MENU               0x00000011L

//  
//
// MessageId: AIC_TACTICAL_MENU
//
// MessageText:
//
//  Tactical (Sub-Menu)%0
//
#define AIC_TACTICAL_MENU                0x00000012L

//  
//
// MessageId: AIC_FLIGHT_MENU
//
// MessageText:
//
//  Flight (Sub-Menu)%0
//
#define AIC_FLIGHT_MENU                  0x00000013L

//  
//
// MessageId: AIC_DIVISION_MENU
//
// MessageText:
//
//  Division (Sub-Menu)%0
//
#define AIC_DIVISION_MENU                0x00000014L

//  
//
// MessageId: AIC_ELEMENT_MENU
//
// MessageText:
//
//  Section (Sub-Menu)%0
//
#define AIC_ELEMENT_MENU                 0x00000015L

//  
//
// MessageId: AIC_WINGMAN_MENU
//
// MessageText:
//
//  Wingman (Sub-Menu)%0
//
#define AIC_WINGMAN_MENU                 0x00000016L

//  
//
// MessageId: AIC_AG_PRIMARY
//
// MessageText:
//
//  Attack Primary Ground Target%0
//
#define AIC_AG_PRIMARY                   0x00000017L

//  
//
// MessageId: AIC_AG_SECONDARY
//
// MessageText:
//
//  Attack Secondary Ground Target%0
//
#define AIC_AG_SECONDARY                 0x00000018L

//  
//
// MessageId: AIC_AG_OPPORTUNITY
//
// MessageText:
//
//  Attack Ground Targets Of Opportunity%0
//
#define AIC_AG_OPPORTUNITY               0x00000019L

//
// MessageId: AIC_ATTACK_AIR_DEF
//
// MessageText:
//
//  Attack Air Defenses%0
//
#define AIC_ATTACK_AIR_DEF               0x0000001AL

//
// MessageId: AIC_ATTACK_WITH
//
// MessageText:
//
//  Attack with ... (Sub-Menu)%0
//
#define AIC_ATTACK_WITH                  0x0000001BL

//
// MessageId: AIC_UNGUIDED_ALL
//
// MessageText:
//
//  Unguided All%0
//
#define AIC_UNGUIDED_ALL                 0x0000001CL

//
// MessageId: AIC_UNGUIDED_HALF
//
// MessageText:
//
//  Unguided, Half%0
//
#define AIC_UNGUIDED_HALF                0x0000001DL

//
// MessageId: AIC_UNGUIDED_SINGLE
//
// MessageText:
//
//  Unguided, Single%0
//
#define AIC_UNGUIDED_SINGLE              0x0000001EL

//
// MessageId: AIC_GUIDED_ALL
//
// MessageText:
//
//  Guided, All%0
//
#define AIC_GUIDED_ALL                   0x0000001FL

//
// MessageId: AIC_GUIDED_HALF
//
// MessageText:
//
//  Guided, Half%0
//
#define AIC_GUIDED_HALF                  0x00000020L

//
// MessageId: AIC_GUIDED_SINGLE
//
// MessageText:
//
//  Guided, Single%0
//
#define AIC_GUIDED_SINGLE                0x00000021L

//
// MessageId: AIC_MISSILE_ALL
//
// MessageText:
//
//  Missile, All%0
//
#define AIC_MISSILE_ALL                  0x00000022L

//
// MessageId: AIC_MISSILE_HALF
//
// MessageText:
//
//  Missile, Half%0
//
#define AIC_MISSILE_HALF                 0x00000023L

//
// MessageId: AIC_MISSILE_SINGLE
//
// MessageText:
//
//  Missile, Single%0
//
#define AIC_MISSILE_SINGLE               0x00000024L

//
// MessageId: AIC_DEFAULT
//
// MessageText:
//
//  Default%0
//
#define AIC_DEFAULT                      0x00000025L

//  
//
// MessageId: AIC_AG_ABORT
//
// MessageText:
//
//  Abort Ground Attack%0
//
#define AIC_AG_ABORT                     0x00000026L

//  
//
// MessageId: AIC_BRACKET_RIGHT
//
// MessageText:
//
//  Bracket Right%0
//
#define AIC_BRACKET_RIGHT                0x00000027L

//  
//
// MessageId: AIC_BRACKET_LEFT
//
// MessageText:
//
//  Bracket Left%0
//
#define AIC_BRACKET_LEFT                 0x00000028L

//  
//
// MessageId: AIC_SPLIT_HIGH
//
// MessageText:
//
//  Split High%0
//
#define AIC_SPLIT_HIGH                   0x00000029L

//  
//
// MessageId: AIC_SPLIT_LOW
//
// MessageText:
//
//  Split Low%0
//
#define AIC_SPLIT_LOW                    0x0000002AL

//  
//
// MessageId: AIC_DRAG_RIGHT
//
// MessageText:
//
//  Draw Right%0
//
#define AIC_DRAG_RIGHT                   0x0000002BL

//  
//
// MessageId: AIC_DRAG_LEFT
//
// MessageText:
//
//  Draw Left%0
//
#define AIC_DRAG_LEFT                    0x0000002CL

//  
//
// MessageId: AIC_BREAK_RIGHT
//
// MessageText:
//
//  Break Right%0
//
#define AIC_BREAK_RIGHT                  0x0000002DL

//  
//
// MessageId: AIC_BREAK_LEFT
//
// MessageText:
//
//  Break Left%0
//
#define AIC_BREAK_LEFT                   0x0000002EL

//  
//
// MessageId: AIC_BREAK_HIGH
//
// MessageText:
//
//  Break High%0
//
#define AIC_BREAK_HIGH                   0x0000002FL

//  
//
// MessageId: AIC_BREAK_LOW
//
// MessageText:
//
//  Break Low%0
//
#define AIC_BREAK_LOW                    0x00000030L

//  
//
// MessageId: AIC_ATTACK_MY_TARGET
//
// MessageText:
//
//  Attack My Target%0
//
#define AIC_ATTACK_MY_TARGET             0x00000031L

//  
//
// MessageId: AIC_HELP_ME
//
// MessageText:
//
//  Help Me%0
//
#define AIC_HELP_ME                      0x00000032L

//  
//
// MessageId: AIC_SAY_POSITION
//
// MessageText:
//
//  Say Position (BULLS)%0
//
#define AIC_SAY_POSITION                 0x00000033L

//  
//
// MessageId: AIC_FLIGHT_CHECK
//
// MessageText:
//
//  Flight/Systems Check%0
//
#define AIC_FLIGHT_CHECK                 0x00000034L

//  
//
// MessageId: AIC_WEAPONS_CHECK
//
// MessageText:
//
//  Weapons Check%0
//
#define AIC_WEAPONS_CHECK                0x00000035L

//  
//
// MessageId: AIC_SAY_FUEL
//
// MessageText:
//
//  Say Fuel%0
//
#define AIC_SAY_FUEL                     0x00000036L

//  
//
// MessageId: AIC_LOOSEN_FORM
//
// MessageText:
//
//  Loosen Current Formation%0
//
#define AIC_LOOSEN_FORM                  0x00000037L

//  
//
// MessageId: AIC_TIGHTEN_FORM
//
// MessageText:
//
//  Tighten Current Formation%0
//
#define AIC_TIGHTEN_FORM                 0x00000038L

//  
//
// MessageId: AIC_WEDGE_FORM
//
// MessageText:
//
//  Wedge Formation%0
//
#define AIC_WEDGE_FORM                   0x00000039L

//  
//
// MessageId: AIC_LINE_ABREAST_FORM
//
// MessageText:
//
//  Line Abreast Formation%0
//
#define AIC_LINE_ABREAST_FORM            0x0000003AL

//  
//
// MessageId: AIC_ECHELON_FORM
//
// MessageText:
//
//  Echelon Formation%0
//
#define AIC_ECHELON_FORM                 0x0000003BL

//  
//
// MessageId: AIC_TRAIL_FORM
//
// MessageText:
//
//  Trail Formation%0
//
#define AIC_TRAIL_FORM                   0x0000003CL

//
// MessageId: AIC_WALL_FORM
//
// MessageText:
//
//  Wall Formation%0
//
#define AIC_WALL_FORM                    0x0000003DL

//
// MessageId: AIC_FLIGHT_REPORTS
//
// MessageText:
//
//  Flight Reports (Sub-Menu)%0
//
#define AIC_FLIGHT_REPORTS               0x0000003EL

//
// MessageId: AIC_CONTACT_REPORTS
//
// MessageText:
//
//  Contact Reports (Sub-Menu)%0
//
#define AIC_CONTACT_REPORTS              0x0000003FL

//
// MessageId: AIC_THREAT_CALLS
//
// MessageText:
//
//  Threat Calls (Sub-Menu)%0
//
#define AIC_THREAT_CALLS                 0x00000040L

//
// MessageId: AIC_WINCHESTER
//
// MessageText:
//
//  Winchester%0
//
#define AIC_WINCHESTER                   0x00000041L

//
// MessageId: AIC_ENGAGEMENT
//
// MessageText:
//
//  Engagement (Sub-Menu)%0
//
#define AIC_ENGAGEMENT                   0x00000042L

//
// MessageId: AIC_DAMAGED
//
// MessageText:
//
//  I've Taken Damage%0
//
#define AIC_DAMAGED                      0x00000043L

//
// MessageId: AIC_BINGO
//
// MessageText:
//
//  Bingo%0
//
#define AIC_BINGO                        0x00000044L

//
// MessageId: AIC_RETURNING_TO_BASE
//
// MessageText:
//
//  Returning To Base%0
//
#define AIC_RETURNING_TO_BASE            0x00000045L

//
// MessageId: AIC_REPORT_SPIKE
//
// MessageText:
//
//  Report Spike%0
//
#define AIC_REPORT_SPIKE                 0x00000046L

//
// MessageId: AIC_REPORT_SORT
//
// MessageText:
//
//  Report Sort%0
//
#define AIC_REPORT_SORT                  0x00000047L

//
// MessageId: AIC_REPORT_CONTACTS_RADAR
//
// MessageText:
//
//  Report Contacts (Radar)%0
//
#define AIC_REPORT_CONTACTS_RADAR        0x00000048L

//
// MessageId: AIC_REPORT_GROUND_TARGET
//
// MessageText:
//
//  Report Ground/Sea Targets%0
//
#define AIC_REPORT_GROUND_TARGET         0x00000049L

//
// MessageId: AIC_REPORT_STROBE
//
// MessageText:
//
//  Report Strobe%0
//
#define AIC_REPORT_STROBE                0x0000004AL

//
// MessageId: AIC_SAM_LAUNCH
//
// MessageText:
//
//  Sam Launch%0
//
#define AIC_SAM_LAUNCH                   0x0000004BL

//
// MessageId: AIC_MISSILE_LAUNCH
//
// MessageText:
//
//  Missile Launch%0
//
#define AIC_MISSILE_LAUNCH               0x0000004CL

//
// MessageId: AIC_AAA_SPOTTED
//
// MessageText:
//
//  AAA Fire Spotted%0
//
#define AIC_AAA_SPOTTED                  0x0000004DL

//
// MessageId: AIC_ENGAGED_OFFENSIVE
//
// MessageText:
//
//  Engaged Offensive%0
//
#define AIC_ENGAGED_OFFENSIVE            0x0000004EL

//
// MessageId: AIC_ENGAGED_DEFENSIVE
//
// MessageText:
//
//  Engaged Defensive%0
//
#define AIC_ENGAGED_DEFENSIVE            0x0000004FL

//
// MessageId: AIC_TARGET_DESTROYED
//
// MessageText:
//
//  Target Destroyed%0
//
#define AIC_TARGET_DESTROYED             0x00000050L

//
// MessageId: AIC_TARGET_DAMAGED
//
// MessageText:
//
//  Target Damaged%0
//
#define AIC_TARGET_DAMAGED               0x00000051L

//
// MessageId: AIC_ATTACKING_GROUND
//
// MessageText:
//
//  Attacking Ground Target%0
//
#define AIC_ATTACKING_GROUND             0x00000052L

//
// %1!1d is the number infront of a menu option line 1) or 2)
//  
//
// MessageId: AIC_FRONT_OF_OPTIONS
//
// MessageText:
//
//  %1!d!)   %0
//
#define AIC_FRONT_OF_OPTIONS             0x00000053L

//
// %1 Holds the callsign for the group
//
// %2 Tells which plane it is is the group (One, Two, à).
//  
//
// MessageId: AIC_CALLSIGN_NUMBER
//
// MessageText:
//
//  %1 %2%0
//
#define AIC_CALLSIGN_NUMBER              0x00000054L

//
// %1 Holds the callsign for the group
//  
//
// MessageId: AIC_FLIGHT_CALLSIGN
//
// MessageText:
//
//  %1 Flight%0
//
#define AIC_FLIGHT_CALLSIGN              0x00000055L

//
// %1!s! is the text for where plane is in the group (ex: two or three).
//
// Where Two is the player's wingman, and three is the leader of the next element
//  
//
// MessageId: AIC_BASIC_ACK
//
// MessageText:
//
//  %1!s!%0
//
#define AIC_BASIC_ACK                    0x00000056L

//
// %1 wingman number
//
// MessageId: AIC_BASIC_ACK_2
//
// MessageText:
//
//  %1 wilco%0
//
#define AIC_BASIC_ACK_2                  0x00000057L

//
// %1 wingman number
//
// MessageId: AIC_BASIC_NEG_1
//
// MessageText:
//
//  %1 negative%0
//
#define AIC_BASIC_NEG_1                  0x00000058L

//
// %1 wingman number
//
// MessageId: AIC_BASIC_NEG_2
//
// MessageText:
//
//  %1 unable to comply%0
//
#define AIC_BASIC_NEG_2                  0x00000059L

//
// %1 wingman number
//
// MessageId: AIC_COPY_ACK
//
// MessageText:
//
//  %1 copy%0
//
#define AIC_COPY_ACK                     0x0000005AL

//
// %1 wingman number
//
// MessageId: AIC_COPY_REFORM
//
// MessageText:
//
//  %1 copy reforming%0
//
#define AIC_COPY_REFORM                  0x0000005BL

//
// %1 wingman number
//
// MessageId: AIC_RETURN_REFORM
//
// MessageText:
//
//  %1 returning to formation%0
//
#define AIC_RETURN_REFORM                0x0000005CL

//
// %1 wingman number
//
// MessageId: AIC_RTB_1
//
// MessageText:
//
//  %1 copy, RTB%0
//
#define AIC_RTB_1                        0x0000005DL

//
// %1 wingman number
//
// MessageId: AIC_RTB_2
//
// MessageText:
//
//  %1 acknowledged, returning to base%0
//
#define AIC_RTB_2                        0x0000005EL

//
// %1 wingman number
//
// MessageId: AIC_RTB_3
//
// MessageText:
//
//  %1 is headed for home%0
//
#define AIC_RTB_3                        0x0000005FL

//
// %1 wingman number
//
// MessageId: AIC_SPREADING_OUT
//
// MessageText:
//
//  %1 spreading out%0
//
#define AIC_SPREADING_OUT                0x00000060L

//
// %1 wingman number
//
// MessageId: AIC_CLOSING_UP
//
// MessageText:
//
//  %1 closing up%0
//
#define AIC_CLOSING_UP                   0x00000061L

//
// %1 wingman number
//
// MessageId: AIC_ORBIT_RESPONSE_1
//
// MessageText:
//
//  %1 copy, anchoring here.%0
//
#define AIC_ORBIT_RESPONSE_1             0x00000062L

//
// %1 wingman number
//
// MessageId: AIC_ORBIT_RESPONSE_2
//
// MessageText:
//
//  %1 roger, orbiting here.%0
//
#define AIC_ORBIT_RESPONSE_2             0x00000063L

//
// %1 wingman number
//
// MessageId: AIC_COVER_RESPONSE_1
//
// MessageText:
//
//  %1 copy, moving to cover%0
//
#define AIC_COVER_RESPONSE_1             0x00000064L

//
// %1 wingman number
//
// MessageId: AIC_COVER_RESPONSE_2
//
// MessageText:
//
//  %1 roger, covering%0
//
#define AIC_COVER_RESPONSE_2             0x00000065L

//  
//
// MessageId: AIC_ZERO
//
// MessageText:
//
//  Zero%0
//
#define AIC_ZERO                         0x00000066L

//  
//
// MessageId: AIC_ONE
//
// MessageText:
//
//  One%0
//
#define AIC_ONE                          0x00000067L

//  
//
// MessageId: AIC_TWO
//
// MessageText:
//
//  Two%0
//
#define AIC_TWO                          0x00000068L

//  
//
// MessageId: AIC_THREE
//
// MessageText:
//
//  Three%0
//
#define AIC_THREE                        0x00000069L

//  
//
// MessageId: AIC_FOUR
//
// MessageText:
//
//  Four%0
//
#define AIC_FOUR                         0x0000006AL

//  
//
// MessageId: AIC_FIVE
//
// MessageText:
//
//  Five%0
//
#define AIC_FIVE                         0x0000006BL

//  
//
// MessageId: AIC_SIX
//
// MessageText:
//
//  Six%0
//
#define AIC_SIX                          0x0000006CL

//  
//
// MessageId: AIC_SEVEN
//
// MessageText:
//
//  Seven%0
//
#define AIC_SEVEN                        0x0000006DL

//  
//
// MessageId: AIC_EIGHT
//
// MessageText:
//
//  Eight%0
//
#define AIC_EIGHT                        0x0000006EL

//  
//
// MessageId: AIC_NINE
//
// MessageText:
//
//  Nine%0
//
#define AIC_NINE                         0x0000006FL

//  
//
// MessageId: AIC_TEN
//
// MessageText:
//
//  Ten%0
//
#define AIC_TEN                          0x00000070L

//
// MessageId: AIC_ELEVEN
//
// MessageText:
//
//  Eleven%0
//
#define AIC_ELEVEN                       0x00000071L

//
// MessageId: AIC_TWELVE
//
// MessageText:
//
//  Twelve%0
//
#define AIC_TWELVE                       0x00000072L

//
// MessageId: AIC_THIRTEEN
//
// MessageText:
//
//  Thirteen%0
//
#define AIC_THIRTEEN                     0x00000073L

//
// MessageId: AIC_FOURTEEN
//
// MessageText:
//
//  Fourteen%0
//
#define AIC_FOURTEEN                     0x00000074L

//  
//
// MessageId: AIC_FIFTEEN
//
// MessageText:
//
//  Fifteen%0
//
#define AIC_FIFTEEN                      0x00000075L

//
// MessageId: AIC_SIXTEEN
//
// MessageText:
//
//  Sixteen%0
//
#define AIC_SIXTEEN                      0x00000076L

//
// MessageId: AIC_SEVENTEEN
//
// MessageText:
//
//  Seventeen%0
//
#define AIC_SEVENTEEN                    0x00000077L

//
// MessageId: AIC_EIGHTEEN
//
// MessageText:
//
//  Eighteen%0
//
#define AIC_EIGHTEEN                     0x00000078L

//
// MessageId: AIC_NINETEEN
//
// MessageText:
//
//  Nineteen%0
//
#define AIC_NINETEEN                     0x00000079L

//  
//
// MessageId: AIC_TWENTY
//
// MessageText:
//
//  Twenty%0
//
#define AIC_TWENTY                       0x0000007AL

//
// MessageId: AIC_TWENTY_ONE
//
// MessageText:
//
//  Twenty-One%0
//
#define AIC_TWENTY_ONE                   0x0000007BL

//
// MessageId: AIC_TWENTY_TWO
//
// MessageText:
//
//  Twenty-Two%0
//
#define AIC_TWENTY_TWO                   0x0000007CL

//
// MessageId: AIC_TWENTY_THREE
//
// MessageText:
//
//  Twenty-Three%0
//
#define AIC_TWENTY_THREE                 0x0000007DL

//
// MessageId: AIC_TWENTY_FOUR
//
// MessageText:
//
//  Twenty-Four%0
//
#define AIC_TWENTY_FOUR                  0x0000007EL

//  
//
// MessageId: AIC_TWENTY_FIVE
//
// MessageText:
//
//  Twenty-Five%0
//
#define AIC_TWENTY_FIVE                  0x0000007FL

//
// MessageId: AIC_TWENTY_SIX
//
// MessageText:
//
//  Twenty-Six%0
//
#define AIC_TWENTY_SIX                   0x00000080L

//
// MessageId: AIC_TWENTY_SEVEN
//
// MessageText:
//
//  Twenty-Seven%0
//
#define AIC_TWENTY_SEVEN                 0x00000081L

//
// MessageId: AIC_TWENTY_EIGHT
//
// MessageText:
//
//  Twenty-Eight%0
//
#define AIC_TWENTY_EIGHT                 0x00000082L

//
// MessageId: AIC_TWENTY_NINE
//
// MessageText:
//
//  Twenty-Nine%0
//
#define AIC_TWENTY_NINE                  0x00000083L

//  
//
// MessageId: AIC_THIRTY
//
// MessageText:
//
//  Thirty%0
//
#define AIC_THIRTY                       0x00000084L

//
// MessageId: AIC_THIRTY_ONE
//
// MessageText:
//
//  Thirty-One%0
//
#define AIC_THIRTY_ONE                   0x00000085L

//
// MessageId: AIC_THIRTY_TWO
//
// MessageText:
//
//  Thirty-Two%0
//
#define AIC_THIRTY_TWO                   0x00000086L

//
// MessageId: AIC_THIRTY_THREE
//
// MessageText:
//
//  Thirty-Three%0
//
#define AIC_THIRTY_THREE                 0x00000087L

//
// MessageId: AIC_THIRTY_FOUR
//
// MessageText:
//
//  Thirty-Four%0
//
#define AIC_THIRTY_FOUR                  0x00000088L

//  
//
// MessageId: AIC_THIRTY_FIVE
//
// MessageText:
//
//  Thirty-Five%0
//
#define AIC_THIRTY_FIVE                  0x00000089L

//
// MessageId: AIC_THIRTY_SIX
//
// MessageText:
//
//  Thirty-Six%0
//
#define AIC_THIRTY_SIX                   0x0000008AL

//
// MessageId: AIC_THIRTY_SEVEN
//
// MessageText:
//
//  Thirty-Seven%0
//
#define AIC_THIRTY_SEVEN                 0x0000008BL

//
// MessageId: AIC_THIRTY_EIGHT
//
// MessageText:
//
//  Thirty-Eight%0
//
#define AIC_THIRTY_EIGHT                 0x0000008CL

//
// MessageId: AIC_THIRTY_NINE
//
// MessageText:
//
//  Thirty-Nine%0
//
#define AIC_THIRTY_NINE                  0x0000008DL

//  
//
// MessageId: AIC_FORTY
//
// MessageText:
//
//  Forty%0
//
#define AIC_FORTY                        0x0000008EL

//
// MessageId: AIC_FORTY_ONE
//
// MessageText:
//
//  Forty-One%0
//
#define AIC_FORTY_ONE                    0x0000008FL

//
// MessageId: AIC_FORTY_TWO
//
// MessageText:
//
//  Forty-Two%0
//
#define AIC_FORTY_TWO                    0x00000090L

//
// MessageId: AIC_FORTY_THREE
//
// MessageText:
//
//  Forty-Three%0
//
#define AIC_FORTY_THREE                  0x00000091L

//
// MessageId: AIC_FORTY_FOUR
//
// MessageText:
//
//  Forty-Four%0
//
#define AIC_FORTY_FOUR                   0x00000092L

//  
//
// MessageId: AIC_FOURTY_FIVE
//
// MessageText:
//
//  Forty-Five%0
//
#define AIC_FOURTY_FIVE                  0x00000093L

//
// MessageId: AIC_FORTY_SIX
//
// MessageText:
//
//  Forty-Six%0
//
#define AIC_FORTY_SIX                    0x00000094L

//
// MessageId: AIC_FORTY_SEVEN
//
// MessageText:
//
//  Forty-Seven%0
//
#define AIC_FORTY_SEVEN                  0x00000095L

//
// MessageId: AIC_FORTY_EIGHT
//
// MessageText:
//
//  Forty-Eight%0
//
#define AIC_FORTY_EIGHT                  0x00000096L

//
// MessageId: AIC_FORTY_NINE
//
// MessageText:
//
//  Forty-Nine%0
//
#define AIC_FORTY_NINE                   0x00000097L

//  
//
// MessageId: AIC_FIFTY
//
// MessageText:
//
//  Fifty%0
//
#define AIC_FIFTY                        0x00000098L

//
// MessageId: AIC_FIFTY_ONE
//
// MessageText:
//
//  Fifty-One%0
//
#define AIC_FIFTY_ONE                    0x00000099L

//
// MessageId: AIC_FIFTY_TWO
//
// MessageText:
//
//  Fifty-Two%0
//
#define AIC_FIFTY_TWO                    0x0000009AL

//
// MessageId: AIC_FIFTY_THREE
//
// MessageText:
//
//  Fifty-Three%0
//
#define AIC_FIFTY_THREE                  0x0000009BL

//
// MessageId: AIC_FIFTY_FOUR
//
// MessageText:
//
//  Fifty-Four%0
//
#define AIC_FIFTY_FOUR                   0x0000009CL

//  
//
// MessageId: AIC_FIFTY_FIVE
//
// MessageText:
//
//  Fifty-Five%0
//
#define AIC_FIFTY_FIVE                   0x0000009DL

//
// MessageId: AIC_FIFTY_SIX
//
// MessageText:
//
//  Fifty-Six%0
//
#define AIC_FIFTY_SIX                    0x0000009EL

//
// MessageId: AIC_FIFTY_SEVEN
//
// MessageText:
//
//  Fifty-Seven%0
//
#define AIC_FIFTY_SEVEN                  0x0000009FL

//
// MessageId: AIC_FIFTY_EIGHT
//
// MessageText:
//
//  Fifty-Eight%0
//
#define AIC_FIFTY_EIGHT                  0x000000A0L

//
// MessageId: AIC_FIFTY_NINE
//
// MessageText:
//
//  Fifty-Nine%0
//
#define AIC_FIFTY_NINE                   0x000000A1L

//  
//
// MessageId: AIC_SIXTY
//
// MessageText:
//
//  Sixty%0
//
#define AIC_SIXTY                        0x000000A2L

//
// MessageId: AIC_SIXTY_ONE
//
// MessageText:
//
//  Sixty-One%0
//
#define AIC_SIXTY_ONE                    0x000000A3L

//
// MessageId: AIC_SIXTY_TWO
//
// MessageText:
//
//  Sixty-Two%0
//
#define AIC_SIXTY_TWO                    0x000000A4L

//
// MessageId: AIC_SIXTY_THREE
//
// MessageText:
//
//  Sixty-Three%0
//
#define AIC_SIXTY_THREE                  0x000000A5L

//
// MessageId: AIC_SIXTY_FOUR
//
// MessageText:
//
//  Sixty-Four%0
//
#define AIC_SIXTY_FOUR                   0x000000A6L

//  
//
// MessageId: AIC_SIXTY_FIVE
//
// MessageText:
//
//  Sixty-Five%0
//
#define AIC_SIXTY_FIVE                   0x000000A7L

//
// MessageId: AIC_SIXTY_SIX
//
// MessageText:
//
//  Sixty-Six%0
//
#define AIC_SIXTY_SIX                    0x000000A8L

//
// MessageId: AIC_SIXTY_SEVEN
//
// MessageText:
//
//  Sixty-Seven%0
//
#define AIC_SIXTY_SEVEN                  0x000000A9L

//
// MessageId: AIC_SIXTY_EIGHT
//
// MessageText:
//
//  Sixty-Eight%0
//
#define AIC_SIXTY_EIGHT                  0x000000AAL

//
// MessageId: AIC_SIXTY_NINE
//
// MessageText:
//
//  Sixty-Nine%0
//
#define AIC_SIXTY_NINE                   0x000000ABL

//  
//
// MessageId: AIC_SEVENTY
//
// MessageText:
//
//  Seventy%0
//
#define AIC_SEVENTY                      0x000000ACL

//
// MessageId: AIC_SEVENTY_ONE
//
// MessageText:
//
//  Seventy-One%0
//
#define AIC_SEVENTY_ONE                  0x000000ADL

//
// MessageId: AIC_SEVENTY_TWO
//
// MessageText:
//
//  Seventy-Two%0
//
#define AIC_SEVENTY_TWO                  0x000000AEL

//
// MessageId: AIC_SEVENTY_THREE
//
// MessageText:
//
//  Seventy-Three%0
//
#define AIC_SEVENTY_THREE                0x000000AFL

//
// MessageId: AIC_SEVENTY_FOUR
//
// MessageText:
//
//  Seventy-Four%0
//
#define AIC_SEVENTY_FOUR                 0x000000B0L

//  
//
// MessageId: AIC_SEVENTY_FIVE
//
// MessageText:
//
//  Seventy-Five%0
//
#define AIC_SEVENTY_FIVE                 0x000000B1L

//
// MessageId: AIC_SEVENTY_SIX
//
// MessageText:
//
//  Seventy-Six%0
//
#define AIC_SEVENTY_SIX                  0x000000B2L

//
// MessageId: AIC_SEVENTY_SEVEN
//
// MessageText:
//
//  Seventy-Seven%0
//
#define AIC_SEVENTY_SEVEN                0x000000B3L

//
// MessageId: AIC_SEVENTY_EIGHT
//
// MessageText:
//
//  Seventy-Eight%0
//
#define AIC_SEVENTY_EIGHT                0x000000B4L

//
// MessageId: AIC_SEVENTY_NINE
//
// MessageText:
//
//  Seventy-Nine%0
//
#define AIC_SEVENTY_NINE                 0x000000B5L

//  
//
// MessageId: AIC_EIGHTY
//
// MessageText:
//
//  Eighty%0
//
#define AIC_EIGHTY                       0x000000B6L

//  
//
// MessageId: AIC_EIGHTY_FIVE
//
// MessageText:
//
//  Eight-Five%0
//
#define AIC_EIGHTY_FIVE                  0x000000B7L

//  
//
// MessageId: AIC_NINETY
//
// MessageText:
//
//  Ninety%0
//
#define AIC_NINETY                       0x000000B8L

//  
//
// MessageId: AIC_NINETY_FIVE
//
// MessageText:
//
//  Ninety-Five%0
//
#define AIC_NINETY_FIVE                  0x000000B9L

//  
//
// MessageId: AIC_ONE_HUNDRED
//
// MessageText:
//
//  One Hundred%0
//
#define AIC_ONE_HUNDRED                  0x000000BAL

//  
//
// MessageId: AIC_ONE_HUNDRED_FIVE
//
// MessageText:
//
//  One Hundred Five%0
//
#define AIC_ONE_HUNDRED_FIVE             0x000000BBL

//  
//
// MessageId: AIC_ONE_HUNDRED_TEN
//
// MessageText:
//
//  One Hundred Ten%0
//
#define AIC_ONE_HUNDRED_TEN              0x000000BCL

//  
//
// MessageId: AIC_ONE_HUNDRED_FIFTEEN
//
// MessageText:
//
//  One Hundred Fifteen%0
//
#define AIC_ONE_HUNDRED_FIFTEEN          0x000000BDL

//  
//
// MessageId: AIC_ONE_HUNDRED_TWENTY
//
// MessageText:
//
//  One Hundred Twenty%0
//
#define AIC_ONE_HUNDRED_TWENTY           0x000000BEL

//  
//
// MessageId: AIC_ONE_HUNDRED_TWENTY_FIVE
//
// MessageText:
//
//  One Hundred Twenty Five%0
//
#define AIC_ONE_HUNDRED_TWENTY_FIVE      0x000000BFL

//  
//
// MessageId: AIC_ONE_HUNDRED_THIRTY
//
// MessageText:
//
//  One Hundred Thirty%0
//
#define AIC_ONE_HUNDRED_THIRTY           0x000000C0L

//  
//
// MessageId: AIC_ONE_HUNDRED_THIRTY_FIVE
//
// MessageText:
//
//  One Hundred Thirty Five%0
//
#define AIC_ONE_HUNDRED_THIRTY_FIVE      0x000000C1L

//  
//
// MessageId: AIC_ONE_HUNDRED_FORTY
//
// MessageText:
//
//  One Hundred Forty%0
//
#define AIC_ONE_HUNDRED_FORTY            0x000000C2L

//  
//
// MessageId: AIC_ONE_HUNDRED_FORTY_FIVE
//
// MessageText:
//
//  One Hundred Forty Five%0
//
#define AIC_ONE_HUNDRED_FORTY_FIVE       0x000000C3L

//  
//
// MessageId: AIC_ONE_HUNDRED_FIFTY
//
// MessageText:
//
//  One Hundred Fifty%0
//
#define AIC_ONE_HUNDRED_FIFTY            0x000000C4L

//  
//
// MessageId: AIC_ONE_HUNDRED_FIFTY_FIVE
//
// MessageText:
//
//  One Hundred Fifty Five%0
//
#define AIC_ONE_HUNDRED_FIFTY_FIVE       0x000000C5L

//
// MessageId: AIC_ONE_HUNDRED_SIXTY
//
// MessageText:
//
//  One Hundred Sixty%0
//
#define AIC_ONE_HUNDRED_SIXTY            0x000000C6L

//
// MessageId: AIC_ONE_HUNDRED_SIXTY_FIVE
//
// MessageText:
//
//  One Hundred Sixty Five%0
//
#define AIC_ONE_HUNDRED_SIXTY_FIVE       0x000000C7L

//
// MessageId: AIC_ONE_HUNDRED_SEVENTY
//
// MessageText:
//
//  One Hundred Seventy%0
//
#define AIC_ONE_HUNDRED_SEVENTY          0x000000C8L

//
// MessageId: AIC_ONE_HUNDRED_SEVENTY_FIVE
//
// MessageText:
//
//  One Hundred Seventy Five%0
//
#define AIC_ONE_HUNDRED_SEVENTY_FIVE     0x000000C9L

//
// MessageId: AIC_ONE_HUNDRED_EIGHTY
//
// MessageText:
//
//  One Hundred Eighty%0
//
#define AIC_ONE_HUNDRED_EIGHTY           0x000000CAL

//
// MessageId: AIC_ONE_HUNDRED_EIGHTY_FIVE
//
// MessageText:
//
//  One Hundred Eighty Five%0
//
#define AIC_ONE_HUNDRED_EIGHTY_FIVE      0x000000CBL

//
// MessageId: AIC_ONE_HUNDRED_NINTY
//
// MessageText:
//
//  One Hundred Ninty%0
//
#define AIC_ONE_HUNDRED_NINTY            0x000000CCL

//
// MessageId: AIC_ONE_HUNDRED_NINTY_FIVE
//
// MessageText:
//
//  One Hundred Ninty Five%0
//
#define AIC_ONE_HUNDRED_NINTY_FIVE       0x000000CDL

//
// MessageId: AIC_TWO_HUNDRED
//
// MessageText:
//
//  Two Hundred%0
//
#define AIC_TWO_HUNDRED                  0x000000CEL

//  
//
// MessageId: AIC_MILES
//
// MessageText:
//
//  miles%0
//
#define AIC_MILES                        0x000000CFL

//  
//
// MessageId: AIC_FEET
//
// MessageText:
//
//  Feet%0
//
#define AIC_FEET                         0x000000D0L

//
// %1 is the position in the group
//  
//
// MessageId: AIC_DISENGAGE_MSG
//
// MessageText:
//
//  %1, copy disengage%0
//
#define AIC_DISENGAGE_MSG                0x000000D1L

//
// %1 is the position in the group
//  
//
// MessageId: AIC_DISENGAGE_MSG_2
//
// MessageText:
//
//  %1, wilco, disengaging%0
//
#define AIC_DISENGAGE_MSG_2              0x000000D2L

//
// %1 is the callsign of the group
//
// %2 is the position in the group
//  
//
// MessageId: AIC_DISENGAGE_MSG_NEG
//
// MessageText:
//
//  %1 %2, unable to disengage.%0
//
#define AIC_DISENGAGE_MSG_NEG            0x000000D3L

//
// %1 is the position in the group
//  
//
// MessageId: AIC_NO_JOY_MSG
//
// MessageText:
//
//  %1, No Joy %0
//
#define AIC_NO_JOY_MSG                   0x000000D4L

//
// %1 is the position in the group
//  
//
// MessageId: AIC_ENGAGE_MSG
//
// MessageText:
//
//  %1, engaged offensive!%0
//
#define AIC_ENGAGE_MSG                   0x000000D5L

//
// %1 is the position in the group
//  
//
// MessageId: AIC_ENGAGE_MSG_2
//
// MessageText:
//
//  %1, engaged!%0
//
#define AIC_ENGAGE_MSG_2                 0x000000D6L

//
// %1 is the position in the group
//  
//
// MessageId: AIC_ENGAGE_MSG_3
//
// MessageText:
//
//  %1, engaging bandits!%0
//
#define AIC_ENGAGE_MSG_3                 0x000000D7L

//
// %1 Wingman number
//  
//
// MessageId: AIC_ENGAGE_READY_MSG_1
//
// MessageText:
//
//  %1 request permission to engage.%0
//
#define AIC_ENGAGE_READY_MSG_1           0x000000D8L

//
// %1 Wingman number
//  
//
// MessageId: AIC_ENGAGE_READY_MSG_2
//
// MessageText:
//
//  %1 ready to engage.%0
//
#define AIC_ENGAGE_READY_MSG_2           0x000000D9L

//
// MessageId: AIC_NORTH
//
// MessageText:
//
//  North%0
//
#define AIC_NORTH                        0x000000DAL

//
// MessageId: AIC_EAST
//
// MessageText:
//
//  East%0
//
#define AIC_EAST                         0x000000DBL

//
// MessageId: AIC_SOUTH
//
// MessageText:
//
//  South%0
//
#define AIC_SOUTH                        0x000000DCL

//
// MessageId: AIC_WEST
//
// MessageText:
//
//  West%0
//
#define AIC_WEST                         0x000000DDL

//
// MessageId: AIC_NORTH_EAST
//
// MessageText:
//
//  Northeast%0
//
#define AIC_NORTH_EAST                   0x000000DEL

//
// MessageId: AIC_SOUTH_EAST
//
// MessageText:
//
//  Southeast%0
//
#define AIC_SOUTH_EAST                   0x000000DFL

//
// MessageId: AIC_SOUTH_WEST
//
// MessageText:
//
//  Southwest%0
//
#define AIC_SOUTH_WEST                   0x000000E0L

//
// MessageId: AIC_NORTH_WEST
//
// MessageText:
//
//  Northwest%0
//
#define AIC_NORTH_WEST                   0x000000E1L

//
// MessageId: AIC_NORTHERN
//
// MessageText:
//
//  Northern%0
//
#define AIC_NORTHERN                     0x000000E2L

//
// MessageId: AIC_EASTERN
//
// MessageText:
//
//  Eastern%0
//
#define AIC_EASTERN                      0x000000E3L

//
// MessageId: AIC_SOUTHERN
//
// MessageText:
//
//  Southern%0
//
#define AIC_SOUTHERN                     0x000000E4L

//
// MessageId: AIC_WESTERN
//
// MessageText:
//
//  Western%0
//
#define AIC_WESTERN                      0x000000E5L

//
// MessageId: AIC_NORTH_EASTERN
//
// MessageText:
//
//  Northeastern%0
//
#define AIC_NORTH_EASTERN                0x000000E6L

//
// MessageId: AIC_SOUTH_EASTERN
//
// MessageText:
//
//  Southeastern%0
//
#define AIC_SOUTH_EASTERN                0x000000E7L

//
// MessageId: AIC_SOUTH_WESTERN
//
// MessageText:
//
//  Southwestern%0
//
#define AIC_SOUTH_WESTERN                0x000000E8L

//
// MessageId: AIC_NORTH_WESTERN
//
// MessageText:
//
//  Northwestern%0
//
#define AIC_NORTH_WESTERN                0x000000E9L

//
// MessageId: AIC_BANDIT
//
// MessageText:
//
//  Bandit%0
//
#define AIC_BANDIT                       0x000000EAL

//
// MessageId: AIC_REJOIN_FLIGHT
//
// MessageText:
//
//  Rejoin Flight%0
//
#define AIC_REJOIN_FLIGHT                0x000000EBL

//
// MessageId: AIC_ATTACK_MY_TARG_POS
//
// MessageText:
//
//  Two, tally your bandit, engaged%0
//
#define AIC_ATTACK_MY_TARG_POS           0x000000ECL

//
// MessageId: AIC_ATTACK_MY_TARG_NEG
//
// MessageText:
//
//  Two, no joy, unable to comply.%0
//
#define AIC_ATTACK_MY_TARG_NEG           0x000000EDL

//
// MessageId: AIC_HELP_ME_POS_1
//
// MessageText:
//
//  Two, visual on lead, on my way.%0
//
#define AIC_HELP_ME_POS_1                0x000000EEL

//
// MessageId: AIC_HELP_ME_POS_2
//
// MessageText:
//
//  Two, visual on lead, tally bandit, 10 seconds.%0
//
#define AIC_HELP_ME_POS_2                0x000000EFL

//
// MessageId: AIC_HELP_ME_NEG_1
//
// MessageText:
//
//  Two, blind, unable to comply.%0
//
#define AIC_HELP_ME_NEG_1                0x000000F0L

//
// MessageId: AIC_HELP_ME_NEG_2
//
// MessageText:
//
//  Two is engaged defensive at this time!%0
//
#define AIC_HELP_ME_NEG_2                0x000000F1L

//
// %1 is the plane's position in the group
//  
//
// MessageId: AIC_IN_HOT
//
// MessageText:
//
//  %1 in hot%0
//
#define AIC_IN_HOT                       0x000000F2L

//
// %1wingman number.
//
// MessageId: AIC_FOX_1
//
// MessageText:
//
//  %1 Fox 1!%0
//
#define AIC_FOX_1                        0x000000F3L

//
// %1wingman number.
//
// MessageId: AIC_FOX_2
//
// MessageText:
//
//  %1 Fox 2!%0
//
#define AIC_FOX_2                        0x000000F4L

//
// %1wingman number.
//
// MessageId: AIC_FOX_3
//
// MessageText:
//
//  %1 Fox 3!%0
//
#define AIC_FOX_3                        0x000000F5L

//
// %1wingman number.
//
// MessageId: AIC_GUNS
//
// MessageText:
//
//  %1 Guns!%0
//
#define AIC_GUNS                         0x000000F6L

//
// %1wingman number.
//  
//
// MessageId: AIC_BOMB_DROP
//
// MessageText:
//
//  %1 bombs gone!%0
//
#define AIC_BOMB_DROP                    0x000000F7L

//
// %1wingman number.
//  
//
// MessageId: AIC_BOMB_DROP_2
//
// MessageText:
//
//  %1 bombs away!%0
//
#define AIC_BOMB_DROP_2                  0x000000F8L

//
// %1 is the plane's position in the group.
//
// MessageId: AIC_MAVERICK_LAUNCH
//
// MessageText:
//
//  %1 Rifle!%0
//
#define AIC_MAVERICK_LAUNCH              0x000000F9L

//
// %1 is the plane's position in the group.
//
// MessageId: AIC_MAVERICK_LAUNCH_2
//
// MessageText:
//
//  %1 Maverick!%0
//
#define AIC_MAVERICK_LAUNCH_2            0x000000FAL

//
// %1 plane's position in group.
//
// %2 cardinal direction he is heading.
//  
//
// MessageId: AIC_END_GRND_ATTACK
//
// MessageText:
//
//  %1 off to the %2%0
//
#define AIC_END_GRND_ATTACK              0x000000FBL

//
// %1 wingman number
//
// %2 position
//
// MessageId: AIC_STROBE
//
// MessageText:
//
//  %1 strobe %2%0
//
#define AIC_STROBE                       0x000000FCL

//
// %1 plane's position in group
//
// %2 group or single
//
// %3 bearing to target (000-359)
//
// %4 range to target
//
// %5 height,medium, low
//
// %6 targets heading
//
// %7 <bandit bandit> or <unkown>
//  
//
// MessageId: AIC_RADAR_CONTACT
//
// MessageText:
//
//  %1, %2 bearing, %3, %4, %5, %6, %7%0
//
#define AIC_RADAR_CONTACT                0x000000FDL

//
// %1 plane's position in group
//
// %2 <Bandit, Bandit> or <unknown>
//
// %3 left/right
//
// %4 o'clock position
//
// %5 high, medium, or level
//
// %6 range to target
//
// %7 closing/flanking/going
//
// MessageId: AIC_VISUAL_CONTACT
//
// MessageText:
//
//  %1 %2, %3, %4 %5, %6, %7%0
//
#define AIC_VISUAL_CONTACT               0x000000FEL

//
// MessageId: AIC_WEEDS
//
// MessageText:
//
//  weeds%0
//
#define AIC_WEEDS                        0x000000FFL

//
// MessageId: AIC_LOW
//
// MessageText:
//
//  low%0
//
#define AIC_LOW                          0x00000100L

//
// MessageId: AIC_MEDIUM
//
// MessageText:
//
//  medium%0
//
#define AIC_MEDIUM                       0x00000101L

//
// MessageId: AIC_MIDDLE
//
// MessageText:
//
//  middle%0
//
#define AIC_MIDDLE                       0x00000102L

//
// MessageId: AIC_HIGH
//
// MessageText:
//
//  high%0
//
#define AIC_HIGH                         0x00000103L

//
// MessageId: AIC_LEVEL
//
// MessageText:
//
//  level%0
//
#define AIC_LEVEL                        0x00000104L

//
// MessageId: AIC_CLOSING
//
// MessageText:
//
//  closing%0
//
#define AIC_CLOSING                      0x00000105L

//
// MessageId: AIC_FLANKING
//
// MessageText:
//
//  flanking%0
//
#define AIC_FLANKING                     0x00000106L

//
// MessageId: AIC_GOING_AWAY
//
// MessageText:
//
//  going away%0
//
#define AIC_GOING_AWAY                   0x00000107L

//
// MessageId: AIC_UNKNOWN
//
// MessageText:
//
//  unknown%0
//
#define AIC_UNKNOWN                      0x00000108L

//
// MessageId: AIC_BANDIT_BANDIT
//
// MessageText:
//
//  bandit bandit%0
//
#define AIC_BANDIT_BANDIT                0x00000109L

//
// MessageId: AIC_AHEAD_POS
//
// MessageText:
//
//  ahead%0
//
#define AIC_AHEAD_POS                    0x0000010AL

//
// MessageId: AIC_FRONT_POS
//
// MessageText:
//
//  front%0
//
#define AIC_FRONT_POS                    0x0000010BL

//
// MessageId: AIC_LEAD_POS
//
// MessageText:
//
//  lead%0
//
#define AIC_LEAD_POS                     0x0000010CL

//
// MessageId: AIC_TRAIL_POS
//
// MessageText:
//
//  trail%0
//
#define AIC_TRAIL_POS                    0x0000010DL

//
// MessageId: AIC_LEFT
//
// MessageText:
//
//  left%0
//
#define AIC_LEFT                         0x0000010EL

//
// MessageId: AIC_RIGHT
//
// MessageText:
//
//  right%0
//
#define AIC_RIGHT                        0x0000010FL

//
// MessageId: AIC_UP
//
// MessageText:
//
//  up%0
//
#define AIC_UP                           0x00000110L

//
// MessageId: AIC_DOWN
//
// MessageText:
//
//  down%0
//
#define AIC_DOWN                         0x00000111L

//
// MessageId: AIC_SINGLE
//
// MessageText:
//
//  single%0
//
#define AIC_SINGLE                       0x00000112L

//
// MessageId: AIC_SAME
//
// MessageText:
//
//  single%0
//
#define AIC_SAME                         0x00000113L

//
// MessageId: AIC_GROUP
//
// MessageText:
//
//  group%0
//
#define AIC_GROUP                        0x00000114L

//
// MessageId: AIC_ADDITIONAL
//
// MessageText:
//
//  additional%0
//
#define AIC_ADDITIONAL                   0x00000115L

//
// MessageId: AIC_FRIENDLY
//
// MessageText:
//
//  friendly%0
//
#define AIC_FRIENDLY                     0x00000116L

//
// MessageId: AIC_ENEMY_STRIKER
//
// MessageText:
//
//  enemy striker%0
//
#define AIC_ENEMY_STRIKER                0x00000117L

//
// MessageId: AIC_ENEMY_CAP
//
// MessageText:
//
//  enemy cap%0
//
#define AIC_ENEMY_CAP                    0x00000118L

//
// MessageId: AIC_ENEMY_CHOPPER
//
// MessageText:
//
//  enemy chopper%0
//
#define AIC_ENEMY_CHOPPER                0x00000119L

//
// MessageId: AIC_LEAD_BANDIT
//
// MessageText:
//
//  Lead bandit%0
//
#define AIC_LEAD_BANDIT                  0x0000011AL

//
// MessageId: AIC_MIDDLE_BANDIT
//
// MessageText:
//
//  Middle bandit%0
//
#define AIC_MIDDLE_BANDIT                0x0000011BL

//
// MessageId: AIC_TRAILING_BANDIT
//
// MessageText:
//
//  Trailing bandit%0
//
#define AIC_TRAILING_BANDIT              0x0000011CL

//
// MessageId: AIC_HELO
//
// MessageText:
//
//  Helo%0
//
#define AIC_HELO                         0x0000011DL

//
// MessageId: AIC_LEAD_HELO
//
// MessageText:
//
//  Lead Helo%0
//
#define AIC_LEAD_HELO                    0x0000011EL

//
// MessageId: AIC_MIDDLE_HELO
//
// MessageText:
//
//  Middle Helo%0
//
#define AIC_MIDDLE_HELO                  0x0000011FL

//
// MessageId: AIC_TRAILING_HELO
//
// MessageText:
//
//  Trailing helo%0
//
#define AIC_TRAILING_HELO                0x00000120L

//
// MessageId: AIC_STRIKER
//
// MessageText:
//
//  Striker%0
//
#define AIC_STRIKER                      0x00000121L

//
// MessageId: AIC_LEAD_STRIKER
//
// MessageText:
//
//  Lead striker%0
//
#define AIC_LEAD_STRIKER                 0x00000122L

//
// MessageId: AIC_MIDDLE_STRIKER
//
// MessageText:
//
//  Middle striker%0
//
#define AIC_MIDDLE_STRIKER               0x00000123L

//
// MessageId: AIC_TRAILING_STRIKER
//
// MessageText:
//
//  Trailing striker%0
//
#define AIC_TRAILING_STRIKER             0x00000124L

//
// %1 wingman call sign
//
// %2 position
//
// MessageId: AIC_STRANGER_MSG
//
// MessageText:
//
//  %1, stranger, %2%0
//
#define AIC_STRANGER_MSG                 0x00000125L

//
// %1 position in the group
//
// MessageId: AIC_CLEAN_MSG
//
// MessageText:
//
//  %1, clean%0
//
#define AIC_CLEAN_MSG                    0x00000126L

//
// %1 plane's position in group.
//
// %2 group or single
//
// %3 bearing to target (000-359)
//
// %4 range to target <miles>
//
// %5 high, medium, or low
//
// %6 targets heading
//
// %7 <bandit bandit> or <unkown>
//
// MessageId: AIC_CONTACT_CALL
//
// MessageText:
//
//  %1, %2 bearing %3, %4, %5, %6, %7%0
//
#define AIC_CONTACT_CALL                 0x00000127L

//
// %1 plane's position in group.
//
// %2 group or single
//
// %3 bearing to target (000-359)
//
// %4 range to target <miles>
//
// %5 high, medium, low
//
// %6 targets heading
//
// MessageId: AIC_ADD_CONTACT
//
// MessageText:
//
//  %1, additional %2, bearing %3, %4, %5, %6%0
//
#define AIC_ADD_CONTACT                  0x00000128L

//
// %1 plane's position in group.
//
// %2 group or single
//
// %3 bearing to target (000-359)
//
// %4 range to target <miles>
//
// %5 high, medium, low
//
// %6 targets heading
//
// MessageId: AIC_NL_CONTACT
//
// MessageText:
//
//  %1, contact %2, bearing %3, %4, %5, %6%0
//
#define AIC_NL_CONTACT                   0x00000129L

//
// %1 wingman number
//
// %2 cardinal2
//
// %3 bandit type
//
// MessageId: AIC_SORTED
//
// MessageText:
//
//  %1, sorted %2 %3.%0
//
#define AIC_SORTED                       0x0000012AL

//
// %1 plane's position in group
//
// MessageId: AIC_GOING_HOT_MSG
//
// MessageText:
//
//  %1, going Hot%0
//
#define AIC_GOING_HOT_MSG                0x0000012BL

//
// %1 plane's position in group
//
// MessageId: AIC_GOING_COLD_MSG
//
// MessageText:
//
//  %1, going Cold%0
//
#define AIC_GOING_COLD_MSG               0x0000012CL

//
// MessageId: AIC_AWACS
//
// MessageText:
//
//  AWACS (Sub-Menu)%0
//
#define AIC_AWACS                        0x0000012DL

//
// MessageId: AIC_AWACS_REQUEST_PICTURE
//
// MessageText:
//
//  (AWACS) Request Picture%0
//
#define AIC_AWACS_REQUEST_PICTURE        0x0000012EL

//
// MessageId: AIC_AWACS_REQUEST_BOGIE_DOPE
//
// MessageText:
//
//  (AWACS) Request Bogie Dope%0
//
#define AIC_AWACS_REQUEST_BOGIE_DOPE     0x0000012FL

//
// MessageId: AIC_AWACS_REQUEST_ASSIST
//
// MessageText:
//
//  (AWACS) Request Assistance%0
//
#define AIC_AWACS_REQUEST_ASSIST         0x00000130L

//
// MessageId: AIC_AWACS_REQUEST_WEASELS
//
// MessageText:
//
//  (AWACS) Request Weasels%0
//
#define AIC_AWACS_REQUEST_WEASELS        0x00000131L

//
// MessageId: AIC_AWACS_ALPHA_CHECKS
//
// MessageText:
//
//  (AWACS) Alpha Checks (Sub-Menu)%0
//
#define AIC_AWACS_ALPHA_CHECKS           0x00000132L

//
// MessageId: AIC_AWACS_ON_STATION
//
// MessageText:
//
//  (AWACS) On Station%0
//
#define AIC_AWACS_ON_STATION             0x00000133L

//
// MessageId: AIC_AWACS_RELIEF
//
// MessageText:
//
//  (AWACS) Need Relief%0
//
#define AIC_AWACS_RELIEF                 0x00000134L

//
// MessageId: AIC_AWACS_BULLSEYE
//
// MessageText:
//
//  (AWACS) Go Bullseye%0
//
#define AIC_AWACS_BULLSEYE               0x00000135L

//
// MessageId: AIC_AWACS_TACTICAL
//
// MessageText:
//
//  (AWACS) Go Tactical%0
//
#define AIC_AWACS_TACTICAL               0x00000136L

//
// MessageId: AIC_AWACS_DECLARE
//
// MessageText:
//
//  (AWACS) Declare%0
//
#define AIC_AWACS_DECLARE                0x00000137L

//
// MessageId: AIC_ALPHA_TO_BULLSEYE
//
// MessageText:
//
//  Alpha Check to Bullseye%0
//
#define AIC_ALPHA_TO_BULLSEYE            0x00000138L

//
// MessageId: AIC_ALPHA_TO_IP
//
// MessageText:
//
//  Alpha Check to  IP%0
//
#define AIC_ALPHA_TO_IP                  0x00000139L

//
// MessageId: AIC_ALPHA_TO_TARGET
//
// MessageText:
//
//  Alpha Check to Target%0
//
#define AIC_ALPHA_TO_TARGET              0x0000013AL

//
// MessageId: AIC_ALPHA_TO_PACKAGE
//
// MessageText:
//
//  Alpha Check to Package%0
//
#define AIC_ALPHA_TO_PACKAGE             0x0000013BL

//
// MessageId: AIC_ALPHA_TO_HOMEPLATE
//
// MessageText:
//
//  Alpha Check to Homeplate%0
//
#define AIC_ALPHA_TO_HOMEPLATE           0x0000013CL

//
// MessageId: AIC_ALPHA_TO_TANKER
//
// MessageText:
//
//  Alpha Check to Tanker%0
//
#define AIC_ALPHA_TO_TANKER              0x0000013DL

//
// MessageId: AIC_ALPHA_TO_DIVERT_FIELD
//
// MessageText:
//
//  Alpha Check to Divert Airfield%0
//
#define AIC_ALPHA_TO_DIVERT_FIELD        0x0000013EL

//
// MessageId: AIC_JSTARS
//
// MessageText:
//
//  (JSTARS)%0
//
#define AIC_JSTARS                       0x0000013FL

//
// MessageId: AIC_JSTARS_MENU
//
// MessageText:
//
//  JSTARS (Sub-Menu)%0
//
#define AIC_JSTARS_MENU                  0x00000140L

//
// MessageId: AIC_JSTARS_CHECK_IN
//
// MessageText:
//
//  (JSTARS) Check In%0
//
#define AIC_JSTARS_CHECK_IN              0x00000141L

//
// MessageId: AIC_JSTARS_CHECK_OUT
//
// MessageText:
//
//  (JSTARS) Check Out%0
//
#define AIC_JSTARS_CHECK_OUT             0x00000142L

//
// MessageId: AIC_JSTARS_TARGET_REQUEST
//
// MessageText:
//
//  (JSTARS) Target Request%0
//
#define AIC_JSTARS_TARGET_REQUEST        0x00000143L

//
// MessageId: AIC_JSTARS_NEW_TARGET
//
// MessageText:
//
//  (JSTARS) New Target Request%0
//
#define AIC_JSTARS_NEW_TARGET            0x00000144L

//
// MessageId: AIC_TANKER_REQUEST
//
// MessageText:
//
//  Tanker Request (Sub-Menu)%0
//
#define AIC_TANKER_REQUEST               0x00000145L

//
// MessageId: AIC_MAX_FUEL
//
// MessageText:
//
//  Max Fuel%0
//
#define AIC_MAX_FUEL                     0x00000146L

//
// MessageId: AIC_MAX_TRAP_FUEL
//
// MessageText:
//
//  Max Trap Weight%0
//
#define AIC_MAX_TRAP_FUEL                0x00000147L

//
// MessageId: AIC_4K_FUEL
//
// MessageText:
//
//  4K Fuel%0
//
#define AIC_4K_FUEL                      0x00000148L

//
// MessageId: AIC_TOWER
//
// MessageText:
//
//  TOWER (Sub-Menu)%0
//
#define AIC_TOWER                        0x00000149L

//
// MessageId: AIC_TOWER_LANDING_REQUEST
//
// MessageText:
//
//  Request Landing%0
//
#define AIC_TOWER_LANDING_REQUEST        0x0000014AL

//
// MessageId: AIC_TOWER_REQUEST_TAKEOFF
//
// MessageText:
//
//  Request Takeoff%0
//
#define AIC_TOWER_REQUEST_TAKEOFF        0x0000014BL

//
// MessageId: AIC_TOWER_REQUEST_TAXI
//
// MessageText:
//
//  Request Taxi%0
//
#define AIC_TOWER_REQUEST_TAXI           0x0000014CL

//
// MessageId: AIC_TOWER_INBOUND
//
// MessageText:
//
//  Inbound%0
//
#define AIC_TOWER_INBOUND                0x0000014DL

//
// MessageId: AIC_SUPER_HORNET_BALL
//
// MessageText:
//
//  Super Hornet Ball%0
//
#define AIC_SUPER_HORNET_BALL            0x0000014EL

//
// MessageId: AIC_TOWER_CLARA
//
// MessageText:
//
//  Clara%0
//
#define AIC_TOWER_CLARA                  0x0000014FL

//
// MessageId: AIC_REQUEST_EMERGENCY
//
// MessageText:
//
//  Request Emergency Approach%0
//
#define AIC_REQUEST_EMERGENCY            0x00000150L

//
// MessageId: AIC_REQUEST_SAR
//
// MessageText:
//
//  Request SAR%0
//
#define AIC_REQUEST_SAR                  0x00000151L

//
// MessageId: AIC_FAC_MENU
//
// MessageText:
//
//  Forward Air Controller (Sub-Menu)%0
//
#define AIC_FAC_MENU                     0x00000152L

//
// MessageId: AIC_FAC_CHECK_IN
//
// MessageText:
//
//  (FAC) Check In%0
//
#define AIC_FAC_CHECK_IN                 0x00000153L

//
// MessageId: AIC_FAC_CHECK_OUT
//
// MessageText:
//
//  (FAC) Check Out%0
//
#define AIC_FAC_CHECK_OUT                0x00000154L

//
// MessageId: AIC_FAC_TARGET
//
// MessageText:
//
//  (FAC) Target Request%0
//
#define AIC_FAC_TARGET                   0x00000155L

//
// MessageId: AIC_FAC_NEW_TARGET
//
// MessageText:
//
//  (FAC) New Target Request%0
//
#define AIC_FAC_NEW_TARGET               0x00000156L

//
// MessageId: AIC_FAC_BLIND
//
// MessageText:
//
//  (FAC) Blind%0
//
#define AIC_FAC_BLIND                    0x00000157L

//
// MessageId: AIC_ESCORTS
//
// MessageText:
//
//  Escorts (Sub-Menu)%0
//
#define AIC_ESCORTS                      0x00000158L

//
// MessageId: AIC_CHECK_IN
//
// MessageText:
//
//  Check in%0
//
#define AIC_CHECK_IN                     0x00000159L

//
// MessageId: AIC_CHECK_OUT
//
// MessageText:
//
//  Check Out%0
//
#define AIC_CHECK_OUT                    0x0000015AL

//
// MessageId: AIC_RELEASE_ESCORTS
//
// MessageText:
//
//  Release Escorts (Sub-Menu)%0
//
#define AIC_RELEASE_ESCORTS              0x0000015BL

//
// MessageId: AIC_ESCORT_ENGAGE_BANDITS
//
// MessageText:
//
//  Engage Bandits (Sub-Menu)%0
//
#define AIC_ESCORT_ENGAGE_BANDITS        0x0000015CL

//
// MessageId: AIC_ESCORT_ENGAGE_DEF
//
// MessageText:
//
//  Engage Air Defenses (Sub-Menu)%0
//
#define AIC_ESCORT_ENGAGE_DEF            0x0000015DL

//
// MessageId: AIC_ESCORT_ENGAGE_GROUND
//
// MessageText:
//
//  Engage Ground Targets (Sub-Menu)%0
//
#define AIC_ESCORT_ENGAGE_GROUND         0x0000015EL

//
// MessageId: AIC_COVER
//
// MessageText:
//
//  (COVER)%0
//
#define AIC_COVER                        0x0000015FL

//
// MessageId: AIC_SEAD
//
// MessageText:
//
//  (SEAD)%0
//
#define AIC_SEAD                         0x00000160L

//
// %1!d! number of option
//
// %2 Callsign of escort
//
// %3 escort type <COVER> or <SEAD>
//
// MessageId: AIC_RELEASE_ESCORT_OPTIONS
//
// MessageText:
//
//  %1!d!) %2 %3%0
//
#define AIC_RELEASE_ESCORT_OPTIONS       0x00000161L

//
// %1 callsign of group
//
// %2 plane's position in contact group
//
// MessageId: AIC_TANKER_PRECONTACT_MSG
//
// MessageText:
//
//  %1 cleared precontact.%0
//
#define AIC_TANKER_PRECONTACT_MSG        0x00000162L

//
// %1 callsign of group
//
// MessageId: AIC_TANKER_ON_MSG
//
// MessageText:
//
//  %1 cleared on.%0
//
#define AIC_TANKER_ON_MSG                0x00000163L

//
// MessageId: AIC_TANKER_DISCONNECT_MSG
//
// MessageText:
//
//  Offload complete, disconnecting%0
//
#define AIC_TANKER_DISCONNECT_MSG        0x00000164L

//
// %1 callsign of group
//
// MessageId: AIC_TANKER_HANDAY_MSG
//
// MessageText:
//
//  Have a nice day, %1.%0
//
#define AIC_TANKER_HANDAY_MSG            0x00000165L

//
// %1 callsign of group
//
// MessageId: AIC_TANKER_HUNTING_MSG
//
// MessageText:
//
//  Good Hunting, %1.%0
//
#define AIC_TANKER_HUNTING_MSG           0x00000166L

//
// MessageId: AIC_TANKER_STABLIZE
//
// MessageText:
//
//  Stabilize%0
//
#define AIC_TANKER_STABLIZE              0x00000167L

//
// %1 callsign of group
//
// %2 plane's position in contact group
//
// %3 callsign of tanker
//
// %4 heading (360 in 10 deg inc)
//
// %5 altitude of tanker
//
// MessageId: AIC_TANKER_HEADING_ALT
//
// MessageText:
//
//  %1 %2, %3 acknowledge, turning to %4, speed 300, angels %5.%0
//
#define AIC_TANKER_HEADING_ALT           0x00000168L

//
// %1 callsign of group
//
// %2 plane's position in contact group
//
// %3 callsign of tanker
//
// %4 heading (360 in 10 deg inc)
//
// MessageId: AIC_TANKER_HEADING_NOALT
//
// MessageText:
//
//  %1 %2, %3 acknowledge, turning to %4, speed 300.%0
//
#define AIC_TANKER_HEADING_NOALT         0x00000169L

//
// MessageId: AIC_TANKER_CHECK_SWITCHES
//
// MessageText:
//
//  Your nose is hot, set systems to standby%0
//
#define AIC_TANKER_CHECK_SWITCHES        0x0000016AL

//
// MessageId: AIC_TANKER_HIT_GOING_IN
//
// MessageText:
//
//  We're hit, going in!%0
//
#define AIC_TANKER_HIT_GOING_IN          0x0000016BL

//
// MessageId: AIC_TANKER_GOING_DOWN
//
// MessageText:
//
//  We're going down…ahhh…!%0
//
#define AIC_TANKER_GOING_DOWN            0x0000016CL

//
// MessageId: AIC_TANKER_BAIL_OUT
//
// MessageText:
//
//  I can't control it! Bail out, bail out now!%0
//
#define AIC_TANKER_BAIL_OUT              0x0000016DL

//
// MessageId: AIC_TANKER_ABANDON_SHIP
//
// MessageText:
//
//  She's not responding… abandon ship!%0
//
#define AIC_TANKER_ABANDON_SHIP          0x0000016EL

//
// %1 callsign of tanker
//
// %2 callsign of group
//
// MessageId: AIC_WSO_REQUEST_TANKER_MSG
//
// MessageText:
//
//  %1 %2 flight request clearance to precontact.%0
//
#define AIC_WSO_REQUEST_TANKER_MSG       0x0000016FL

//
// %1 callsign of tanker
//
// %2 callsign of group
//
// %3 plane's position in contact group
//
// MessageId: AIC_WSO_PRECONTACT_READY_MSG_OLD
//
// MessageText:
//
//  %1 %2 %3 precontact ready.%0
//
#define AIC_WSO_PRECONTACT_READY_MSG_OLD 0x00000170L

//
// %1 callsign of tanker
//
// %2 callsign of flight
//
// MessageId: AIC_WSO_FUELING_MSG
//
// MessageText:
//
//  Receiving Fuel.%0
//
#define AIC_WSO_FUELING_MSG              0x00000171L

//
// %1 callsign of tanker
//
// %2 callsign of flight
//
// MessageId: AIC_WSO_DISCONNECT_MSG
//
// MessageText:
//
//  Disconnect.%0
//
#define AIC_WSO_DISCONNECT_MSG           0x00000172L

//
// %1 callsign of tanker
//
// %2 callsign of flight
//
// MessageId: AIC_WSO_TANKER_ACKNOWLEDGED_MSG
//
// MessageText:
//
//  %1, %2 flight, acknowledged.%0
//
#define AIC_WSO_TANKER_ACKNOWLEDGED_MSG  0x00000173L

//
// %1 callsign of tanker
//
// %2 callsign of flight
//
// MessageId: AIC_WSO_TANKER_THANKS_MSG
//
// MessageText:
//
//  %1, %2 flight, thanks for the gas.%0
//
#define AIC_WSO_TANKER_THANKS_MSG        0x00000174L

//
// %1 distance to target
//
// MessageId: AIC_WSO_MILES_TO_TARGET
//
// MessageText:
//
//  %1 miles%0
//
#define AIC_WSO_MILES_TO_TARGET          0x00000175L

//
// MessageId: AIC_WSO_MILE_TO_TARGET
//
// MessageText:
//
//  One Mile%0
//
#define AIC_WSO_MILE_TO_TARGET           0x00000176L

//
// MessageId: AIC_WSO_SLOWER
//
// MessageText:
//
//  Slower%0
//
#define AIC_WSO_SLOWER                   0x00000177L

//
// MessageId: AIC_WSO_WATCH_CLOSURE
//
// MessageText:
//
//  Watch your closure%0
//
#define AIC_WSO_WATCH_CLOSURE            0x00000178L

//
// %1 callsign for tanker
//
// MessageId: AIC_WSO_BREAKAWAY
//
// MessageText:
//
//  %1 Breakaway!, Breakaway!, Breakaway!.%0
//
#define AIC_WSO_BREAKAWAY                0x00000179L

//
// MessageId: AIC_WSO_EMIS_SELECTED
//
// MessageText:
//
//  E-Mis selected.%0
//
#define AIC_WSO_EMIS_SELECTED            0x0000017AL

//
// %1 Wingman callsign
//
// %2 clock position
//
// %3 high/low
//
// %4 aspect
//
// MessageId: AIC_WING_ENEMY_SPOT_1
//
// MessageText:
//
//  %1, Bandit Bandit, %2 %3, %4%0
//
#define AIC_WING_ENEMY_SPOT_1            0x0000017BL

//
// %1 Clock position
//
// %2 high/low
//
// MessageId: AIC_WSO_ENEMY_SPOT_1
//
// MessageText:
//
//  Bandit, %1 %2%0
//
#define AIC_WSO_ENEMY_SPOT_1             0x0000017CL

//
// %1 Wingman callsign
//
// %2 clock position
//
// %3 high/low
//
// %4 aspect
//
// MessageId: AIC_WING_ENEMY_SPOT_2
//
// MessageText:
//
//  %1, has Bandits at, %2 %3, %4%0
//
#define AIC_WING_ENEMY_SPOT_2            0x0000017DL

//
// %1clock position
//
// %2 high/low
//
// MessageId: AIC_WSO_CHOPPER_SPOT_1
//
// MessageText:
//
//  Enemy chopper, %2 %3%0
//
#define AIC_WSO_CHOPPER_SPOT_1           0x0000017EL

//
// %1 Wingman callsign
//
// %2 clock position
//
// %3 high/low
//
// %4 aspect
//
// MessageId: AIC_WING_CHOPPER_SPOT_1
//
// MessageText:
//
//  %1, enemy chopper, %2 %3, %4%0
//
#define AIC_WING_CHOPPER_SPOT_1          0x0000017FL

//
// %1 Wingman callsign
//
// %2 clock position
//
// %3 high/low
//
// %4 aspect
//
// MessageId: AIC_WING_CHOPPER_SPOT_2
//
// MessageText:
//
//  %1, enemy choppers at, %2 %3, %4%0
//
#define AIC_WING_CHOPPER_SPOT_2          0x00000180L

//
// %1 position
//
// MessageId: AIC_WSO_AAA_SPOT_1
//
// MessageText:
//
//  Triple A at, %1%0
//
#define AIC_WSO_AAA_SPOT_1               0x00000181L

//
// %1 position
//
// MessageId: AIC_WSO_AAA_SPOT_2
//
// MessageText:
//
//  Guns below us at %1%0
//
#define AIC_WSO_AAA_SPOT_2               0x00000182L

//
// MessageId: AIC_WSO_AAA_LEFT
//
// MessageText:
//
//  Lots of Triple A left%0
//
#define AIC_WSO_AAA_LEFT                 0x00000183L

//
// MessageId: AIC_WSO_AAA_RIGHT
//
// MessageText:
//
//  Lots of Triple A right.%0
//
#define AIC_WSO_AAA_RIGHT                0x00000184L

//
// MessageId: AIC_WSO_AAA_BEHIND
//
// MessageText:
//
//  Lots of Triple A behind us%0
//
#define AIC_WSO_AAA_BEHIND               0x00000185L

//
// MessageId: AIC_WSO_AAA_AROUND
//
// MessageText:
//
//  Lots of Triple A all around us%0
//
#define AIC_WSO_AAA_AROUND               0x00000186L

//
// MessageId: AIC_WSO_AAA_AHEAD
//
// MessageText:
//
//  Lots of Triple A ahead%0
//
#define AIC_WSO_AAA_AHEAD                0x00000187L

//
// %1 Wingman callsign
//
// %2 position
//
// MessageId: AIC_WING_AAA_SPOT_1
//
// MessageText:
//
//  %1, triple A at, %2%0
//
#define AIC_WING_AAA_SPOT_1              0x00000188L

//
// %1 Wingman callsign
//
// %2 position
//
// MessageId: AIC_WING_AAA_SPOT_2
//
// MessageText:
//
//  %1, ground fire at, %2%0
//
#define AIC_WING_AAA_SPOT_2              0x00000189L

//
// %1 position
//
// %2 break right/left
//
// MessageId: AIC_WSO_SAM_SPOT_1
//
// MessageText:
//
//  SAM launch %1%0
//
#define AIC_WSO_SAM_SPOT_1               0x0000018AL

//
// %1 position
//
// %2 break right/left
//
// MessageId: AIC_WSO_SAM_SPOT_2
//
// MessageText:
//
//  SAM! SAM! %1%0
//
#define AIC_WSO_SAM_SPOT_2               0x0000018BL

//
// %1 position
//
// %2 break right/left
//
// MessageId: AIC_WSO_SAM_SPOT_3
//
// MessageText:
//
//  Missile launch, %1%0
//
#define AIC_WSO_SAM_SPOT_3               0x0000018CL

//
// %1 Wingman callsign
//
// %2 position
//
// MessageId: AIC_WING_SAM_SPOT_1
//
// MessageText:
//
//  %1, SAM site at, %2%0
//
#define AIC_WING_SAM_SPOT_1              0x0000018DL

//
// %1 Wingman callsign
//
// %2 position
//
// MessageId: AIC_WING_SAM_SPOT_2
//
// MessageText:
//
//  %1, SAM launch %2%!%0
//
#define AIC_WING_SAM_SPOT_2              0x0000018EL

//
// %1 Wingman callsign
//
// %2 position
//
// MessageId: AIC_WING_SAM_SPOT_3
//
// MessageText:
//
//  %1, SAM %2%!%0
//
#define AIC_WING_SAM_SPOT_3              0x0000018FL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_SAM
//
// MessageText:
//
//  %1 is engaged defensive SAM%0
//
#define AIC_WING_DEFENSIVE_SAM           0x00000190L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_ROLAND
//
// MessageText:
//
//  %1 is engaged defensive Roland%0
//
#define AIC_WING_DEFENSIVE_ROLAND        0x00000191L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_HAWK
//
// MessageText:
//
//  %1 is engaged defensive Hawk%0
//
#define AIC_WING_DEFENSIVE_HAWK          0x00000192L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_SA_2
//
// MessageText:
//
//  %1 is engaged defensive SA-2%0
//
#define AIC_WING_DEFENSIVE_SA_2          0x00000193L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_SA_3
//
// MessageText:
//
//  %1 is engaged defensive SA-3%0
//
#define AIC_WING_DEFENSIVE_SA_3          0x00000194L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_SA_6
//
// MessageText:
//
//  %1 is engaged defensive SA-6%0
//
#define AIC_WING_DEFENSIVE_SA_6          0x00000195L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_SA_7
//
// MessageText:
//
//  %1 is engaged defensive SA-7%0
//
#define AIC_WING_DEFENSIVE_SA_7          0x00000196L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_SA_8
//
// MessageText:
//
//  %1 is engaged defensive SA-8%0
//
#define AIC_WING_DEFENSIVE_SA_8          0x00000197L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_SA_9
//
// MessageText:
//
//  %1 is engaged defensive SA-9%0
//
#define AIC_WING_DEFENSIVE_SA_9          0x00000198L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_SA_10
//
// MessageText:
//
//  %1 is engaged defensive SA-10%0
//
#define AIC_WING_DEFENSIVE_SA_10         0x00000199L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_SA_13
//
// MessageText:
//
//  %1 is engaged defensive SA-13%0
//
#define AIC_WING_DEFENSIVE_SA_13         0x0000019AL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_ATOLL
//
// MessageText:
//
//  %1 is engaged defensive ATOLL%0
//
#define AIC_WING_DEFENSIVE_ATOLL         0x0000019BL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_APEX
//
// MessageText:
//
//  %1 is engaged defensive APEX%0
//
#define AIC_WING_DEFENSIVE_APEX          0x0000019CL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_ARCHER
//
// MessageText:
//
//  %1 is engaged defensive ARCHER%0
//
#define AIC_WING_DEFENSIVE_ARCHER        0x0000019DL

//
// MessageId: AIC_WSO_CHAFF
//
// MessageText:
//
//  Chaff!  Chaff!%0
//
#define AIC_WSO_CHAFF                    0x0000019EL

//
// MessageId: AIC_WSO_FLARE
//
// MessageText:
//
//  Flare!  Flare!%0
//
#define AIC_WSO_FLARE                    0x0000019FL

//
// MessageId: AIC_WSO_MUSIC_ON
//
// MessageText:
//
//  Music On%0
//
#define AIC_WSO_MUSIC_ON                 0x000001A0L

//
// MessageId: AIC_WSO_MUSIC_OFF
//
// MessageText:
//
//  Music Off%0
//
#define AIC_WSO_MUSIC_OFF                0x000001A1L

//
// MessageId: AIC_WSO_AAA_HITS_1
//
// MessageText:
//
//  We're taking triple-A hits!%0
//
#define AIC_WSO_AAA_HITS_1               0x000001A2L

//
// MessageId: AIC_WSO_AAA_HITS_2
//
// MessageText:
//
//  We've been hit!%0
//
#define AIC_WSO_AAA_HITS_2               0x000001A3L

//
// MessageId: AIC_WSO_AAA_HITS_3
//
// MessageText:
//
//  We're taking heavy triple-A fire!%0
//
#define AIC_WSO_AAA_HITS_3               0x000001A4L

//
// MessageId: AIC_WSO_AAA_HITS_4
//
// MessageText:
//
//  Get above this stuff!%0
//
#define AIC_WSO_AAA_HITS_4               0x000001A5L

//
// MessageId: AIC_WSO_CANNON_HIT_1
//
// MessageText:
//
//  We're hit!%0
//
#define AIC_WSO_CANNON_HIT_1             0x000001A6L

//
// MessageId: AIC_WSO_CANNON_HIT_2
//
// MessageText:
//
//  We're taking fire!%0
//
#define AIC_WSO_CANNON_HIT_2             0x000001A7L

//
// MessageId: AIC_WSO_CANNON_HIT_3
//
// MessageText:
//
//  We're taking cannon fire!%0
//
#define AIC_WSO_CANNON_HIT_3             0x000001A8L

//
// MessageId: AIC_WSO_MISSILE_HIT_1
//
// MessageText:
//
//  That missile just hit us!%0
//
#define AIC_WSO_MISSILE_HIT_1            0x000001A9L

//
// MessageId: AIC_WSO_MISSILE_HIT_2
//
// MessageText:
//
//  I think we just took a missile hit!%0
//
#define AIC_WSO_MISSILE_HIT_2            0x000001AAL

//
// MessageId: AIC_WSO_MISSILE_HIT_3
//
// MessageText:
//
//  We've been hit!%0
//
#define AIC_WSO_MISSILE_HIT_3            0x000001ABL

//
// MessageId: AIC_WSO_MISSILE_HIT_4
//
// MessageText:
//
//  Uh oh!  That hurt!%0
//
#define AIC_WSO_MISSILE_HIT_4            0x000001ACL

//
// MessageId: AIC_WSO_FUEL_LEAK
//
// MessageText:
//
//  We've got a fuel leak!%0
//
#define AIC_WSO_FUEL_LEAK                0x000001ADL

//
// MessageId: AIC_WSO_ON_FIRE
//
// MessageText:
//
//  We are on fire!%0
//
#define AIC_WSO_ON_FIRE                  0x000001AEL

//
// MessageId: AIC_WSO_HYDRAULIC_DAMAGE
//
// MessageText:
//
//  We're losing hydraulic pressure!%0
//
#define AIC_WSO_HYDRAULIC_DAMAGE         0x000001AFL

//
// MessageId: AIC_WSO_ENGINE_ONE_DAMAGE
//
// MessageText:
//
//  Engine One is damaged!%0
//
#define AIC_WSO_ENGINE_ONE_DAMAGE        0x000001B0L

//
// MessageId: AIC_WSO_ENGINE_TWO_DAMAGE
//
// MessageText:
//
//  Engine Two is damaged!%0
//
#define AIC_WSO_ENGINE_TWO_DAMAGE        0x000001B1L

//
// MessageId: AIC_WSO_DEEP_DOO
//
// MessageText:
//
//  We're in deep doo doo now!%0
//
#define AIC_WSO_DEEP_DOO                 0x000001B2L

//
// MessageId: AIC_WSO_CHECK_PANNEL
//
// MessageText:
//
//  Check your caution panel, we've lost some systems!%0
//
#define AIC_WSO_CHECK_PANNEL             0x000001B3L

//
// MessageId: AIC_WSO_HURT_BAD
//
// MessageText:
//
//  We're hurt bad!%0
//
#define AIC_WSO_HURT_BAD                 0x000001B4L

//
// MessageId: AIC_WSO_EJECT_EJECT
//
// MessageText:
//
//  We're going in!  Eject! Eject!%0
//
#define AIC_WSO_EJECT_EJECT              0x000001B5L

//
// MessageId: AIC_WSO_WHAT_WAS_THAT
//
// MessageText:
//
//  What was that?%0
//
#define AIC_WSO_WHAT_WAS_THAT            0x000001B6L

//
// MessageId: AIC_WSO_LOST_TEWS
//
// MessageText:
//
//  We've lost the Tews!%0
//
#define AIC_WSO_LOST_TEWS                0x000001B7L

//
// MessageId: AIC_WSO_RADAR_OUT
//
// MessageText:
//
//  The radar is out!%0
//
#define AIC_WSO_RADAR_OUT                0x000001B8L

//
// MessageId: AIC_WSO_COMPUTER_GONE
//
// MessageText:
//
//  Looks like the central computer is gone.%0
//
#define AIC_WSO_COMPUTER_GONE            0x000001B9L

//
// MessageId: AIC_WSO_TARGETING_DEAD
//
// MessageText:
//
//  The targeting pod is dead!%0
//
#define AIC_WSO_TARGETING_DEAD           0x000001BAL

//
// MessageId: AIC_WSO_NAV_HIT
//
// MessageText:
//
//  The Nav pod must have been hit!%0
//
#define AIC_WSO_NAV_HIT                  0x000001BBL

//
// MessageId: AIC_WSO_AUTOPILOT_SHOT
//
// MessageText:
//
//  The autopilot is shot!%0
//
#define AIC_WSO_AUTOPILOT_SHOT           0x000001BCL

//
// MessageId: AIC_WSO_PACS_OUT
//
// MessageText:
//
//  Pacs is out!%0
//
#define AIC_WSO_PACS_OUT                 0x000001BDL

//
// MessageId: AIC_WSO_CABIN_PRESSURE
//
// MessageText:
//
//  We're losing cabin pressure!%0
//
#define AIC_WSO_CABIN_PRESSURE           0x000001BEL

//
// MessageId: AIC_WSO_LOST_PRESSUREIZATION
//
// MessageText:
//
//  Take us lower!  We've lost pressurization!%0
//
#define AIC_WSO_LOST_PRESSUREIZATION     0x000001BFL

//
// %1 Clock position
//
// MessageId: AIC_WSO_SPIKE
//
// MessageText:
//
//  Spike, %1%0
//
#define AIC_WSO_SPIKE                    0x000001C0L

//
// %1 Clock position
//
// MessageId: AIC_WSO_MUD_SPIKE
//
// MessageText:
//
//  Mud Spike at %1%0
//
#define AIC_WSO_MUD_SPIKE                0x000001C1L

//
// %1 Clock position
//
// MessageId: AIC_WSO_GCI_SPIKE
//
// MessageText:
//
//  Spike GCI at %1%0
//
#define AIC_WSO_GCI_SPIKE                0x000001C2L

//
// %1 Clock position
//
// MessageId: AIC_WSO_SAM_SPIKE
//
// MessageText:
//
//  Spike SAM at %1%0
//
#define AIC_WSO_SAM_SPIKE                0x000001C3L

//
// %1 Clock position
//
// MessageId: AIC_WSO_AAA_SPIKE
//
// MessageText:
//
//  Spike Triple A at %1%0
//
#define AIC_WSO_AAA_SPIKE                0x000001C4L

//
// %1 Clock position
//
// MessageId: AIC_WSO_MIG_SPIKE
//
// MessageText:
//
//  Spike MiG at %1%0
//
#define AIC_WSO_MIG_SPIKE                0x000001C5L

//
// %1 Clock position
//
// MessageId: AIC_WSO_MISSILE_LAUNCH
//
// MessageText:
//
//  Missile Launch, %1%0
//
#define AIC_WSO_MISSILE_LAUNCH           0x000001C6L

//
// %1 Clock position
//
// MessageId: AIC_WSO_MUD_LOCK
//
// MessageText:
//
//  Mud Lock on, %1!%0
//
#define AIC_WSO_MUD_LOCK                 0x000001C7L

//
// %1 Clock position
//
// MessageId: AIC_BUDDY_SPIKE
//
// MessageText:
//
//  Buddy Spike at %1%0
//
#define AIC_BUDDY_SPIKE                  0x000001C8L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_HITS_FROM_GROUND
//
// MessageText:
//
//  %1 has taken hits from the ground!%0
//
#define AIC_WING_HITS_FROM_GROUND        0x000001C9L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_WE_HIT
//
// MessageText:
//
//  %1 we've been hit!%0
//
#define AIC_WING_WE_HIT                  0x000001CAL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_WE_HIT_HURT
//
// MessageText:
//
//  %1 we're hurt, assessing damage now!%0
//
#define AIC_WING_WE_HIT_HURT             0x000001CBL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_WE_HIT_MAJOR
//
// MessageText:
//
//  %1 we took some major hits!%0
//
#define AIC_WING_WE_HIT_MAJOR            0x000001CCL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_EJECTING
//
// MessageText:
//
//  %1 ejecting now!%0
//
#define AIC_WING_EJECTING                0x000001CDL

//
// %1 player/wingman callsign
//
// MessageId: AIC_WING_EJECT_CALL_1
//
// MessageText:
//
//  %1 you're on fire!  Eject!  Eject%0
//
#define AIC_WING_EJECT_CALL_1            0x000001CEL

//
// %1 player/wingman callsign
//
// MessageId: AIC_WING_EJECT_CALL_2
//
// MessageText:
//
//  %1 Eject!  Eject!%0
//
#define AIC_WING_EJECT_CALL_2            0x000001CFL

//
// %1 player/wingman callsign
//
// MessageId: AIC_WING_EJECT_CALL_3
//
// MessageText:
//
//  %1 get out of there!  Eject%0
//
#define AIC_WING_EJECT_CALL_3            0x000001D0L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_1
//
// MessageText:
//
//  %1 has a Mud Spike at %2%0
//
#define AIC_WING_RADAR_DETECT_1          0x000001D1L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_2
//
// MessageText:
//
//  %1 Spike GCI %2%0
//
#define AIC_WING_RADAR_DETECT_2          0x000001D2L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_3
//
// MessageText:
//
//  %1 Spike SAM %2%0
//
#define AIC_WING_RADAR_DETECT_3          0x000001D3L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_4
//
// MessageText:
//
//  %1 Spike Triple A %2%0
//
#define AIC_WING_RADAR_DETECT_4          0x000001D4L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_5
//
// MessageText:
//
//  %1 Spike MiG %2%0
//
#define AIC_WING_RADAR_DETECT_5          0x000001D5L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_6
//
// MessageText:
//
//  %1 Missile Launch %2%0
//
#define AIC_WING_RADAR_DETECT_6          0x000001D6L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_7
//
// MessageText:
//
//  %1 Mud Lock on %2%0
//
#define AIC_WING_RADAR_DETECT_7          0x000001D7L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_8
//
// MessageText:
//
//  %1 Spike Spike %2%0
//
#define AIC_WING_RADAR_DETECT_8          0x000001D8L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_9
//
// MessageText:
//
//  %1 Buddy Spike %2%0
//
#define AIC_WING_RADAR_DETECT_9          0x000001D9L

//
// %1 wingman number
//
// MessageId: AIC_WING_TARGETS_SIGHTED_IN_HOT
//
// MessageText:
//
//  %1, targets in sight, in hot%0
//
#define AIC_WING_TARGETS_SIGHTED_IN_HOT  0x000001DAL

//
// %1 wingman number
//
// MessageId: AIC_WING_EGAGING_GOMERS
//
// MessageText:
//
//  %1, gomers spotted, engaging%0
//
#define AIC_WING_EGAGING_GOMERS          0x000001DBL

//
// %1 wingman number
//
// MessageId: AIC_WING_GROUND_DESTROYED
//
// MessageText:
//
//  %1, ground targets destroyed%0
//
#define AIC_WING_GROUND_DESTROYED        0x000001DCL

//
// %1 wingman number
//
// MessageId: AIC_WING_TARGET_DESTROYED_REFORMING
//
// MessageText:
//
//  %1, target destroyed, reforming%0
//
#define AIC_WING_TARGET_DESTROYED_REFORMING 0x000001DDL

//
// %1 wingman number
//
// MessageId: AIC_WING_REQUEST_ATTACK
//
// MessageText:
//
//  %1, request permission to attack.%0
//
#define AIC_WING_REQUEST_ATTACK          0x000001DEL

//
// %1 wingman number
//
// MessageId: AIC_WING_READY_TO_ATTACK
//
// MessageText:
//
//  %1, ready to start our attack run%0
//
#define AIC_WING_READY_TO_ATTACK         0x000001DFL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_BREAK_RIGHT
//
// MessageText:
//
//  %1, break right%0
//
#define AIC_WING_BREAK_RIGHT             0x000001E0L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_BREAK_LEFT
//
// MessageText:
//
//  %1, break left%0
//
#define AIC_WING_BREAK_LEFT              0x000001E1L

//
// %1 Wingman number
//
// MessageId: AIC_WING_SPLASH_1
//
// MessageText:
//
//  %1 Splash my bandit!%0
//
#define AIC_WING_SPLASH_1                0x000001E2L

//
// %1 Wingman number
//
// %2 number
//
// MessageId: AIC_WING_SPLASH_2
//
// MessageText:
//
//  %1 splash %2%0
//
#define AIC_WING_SPLASH_2                0x000001E3L

//
// %1 Wingman number
//
// MessageId: AIC_WING_SPLASH_3
//
// MessageText:
//
//  %1 bandit destroyed!%0
//
#define AIC_WING_SPLASH_3                0x000001E4L

//
// MessageId: AIC_PLAYER_KILL_1
//
// MessageText:
//
//  Good kill lead!%0
//
#define AIC_PLAYER_KILL_1                0x000001E5L

//
// %1 Wingman callsign
//
// MessageId: AIC_PLAYER_KILL_2
//
// MessageText:
//
//  %1 confirmed, that's a kill%0
//
#define AIC_PLAYER_KILL_2                0x000001E6L

//
// %1 Player callsign
//
// MessageId: AIC_PLAYER_KILL_3
//
// MessageText:
//
//  Good kill %1%0
//
#define AIC_PLAYER_KILL_3                0x000001E7L

//
// %1 Wingman number
//
// %2 string of problems
//
// MessageId: AIC_REPORT_PROBLEM
//
// MessageText:
//
//  %1 %2.%0
//
#define AIC_REPORT_PROBLEM               0x000001E8L

//
// MessageId: AI_SINGLE_ENGINE
//
// MessageText:
//
//  single engine%0
//
#define AI_SINGLE_ENGINE                 0x000001E9L

//
// MessageId: AI_GADGET_BENT
//
// MessageText:
//
//  gadget bent%0
//
#define AI_GADGET_BENT                   0x000001EAL

//
// MessageId: AI_BINGO_FUEL
//
// MessageText:
//
//  bingo fuel%0
//
#define AI_BINGO_FUEL                    0x000001EBL

//
// MessageId: AI_HYDRAULICS_BENT
//
// MessageText:
//
//  hydraulics bent%0
//
#define AI_HYDRAULICS_BENT               0x000001ECL

//
// %1 Wingman number
//
// %2 radar missile string
//
// %3 heat missile string
//
// MessageId: AIC_WEAPONS_GUN
//
// MessageText:
//
//  %1, %2, %3, and gun%0
//
#define AIC_WEAPONS_GUN                  0x000001EDL

//
// %1 Wingman number
//
// %2 radar missile string
//
// %3 heat missile string
//
// MessageId: AIC_WEAPONS_NO_GUN
//
// MessageText:
//
//  %1, %2, %3, and zero gun%0
//
#define AIC_WEAPONS_NO_GUN               0x000001EEL

//
// %1 number of missiles
//
// MessageId: AIC_RADAR_COUNT
//
// MessageText:
//
//  %1 radar%0
//
#define AIC_RADAR_COUNT                  0x000001EFL

//
// %1 number of missiles
//
// MessageId: AIC_HEAT_COUNT
//
// MessageText:
//
//  %1 heat%0
//
#define AIC_HEAT_COUNT                   0x000001F0L

//
// %1 Clock position
//
// MessageId: AIC_POSITION
//
// MessageText:
//
//  %1 o'clock%0
//
#define AIC_POSITION                     0x000001F1L

//
// %1 Clock position str
//
// %2 <hight/ /low>
//
// MessageId: AIC_POSITION_HL
//
// MessageText:
//
//  %1 %2%0
//
#define AIC_POSITION_HL                  0x000001F2L

//
// MessageId: AIC_ENEMY_EXTENDING
//
// MessageText:
//
//  He's extending.%0
//
#define AIC_ENEMY_EXTENDING              0x000001F3L

//
// MessageId: AIC_ENEMY_TURNING_BACK_TO_US
//
// MessageText:
//
//  He's turning back into us.%0
//
#define AIC_ENEMY_TURNING_BACK_TO_US     0x000001F4L

//
// MessageId: AIC_ENEMY_BREAKING_RIGHT
//
// MessageText:
//
//  He's breaking right.%0
//
#define AIC_ENEMY_BREAKING_RIGHT         0x000001F5L

//
// MessageId: AIC_ENEMY_BREAKING_LEFT
//
// MessageText:
//
//  He's breaking left.%0
//
#define AIC_ENEMY_BREAKING_LEFT          0x000001F6L

//
// MessageId: AIC_ENEMY_CONVERT_SIX
//
// MessageText:
//
//  He's converting to our six!%0
//
#define AIC_ENEMY_CONVERT_SIX            0x000001F7L

//
// MessageId: AIC_ENEMY_ON_SIX
//
// MessageText:
//
//  He's on our six!%0
//
#define AIC_ENEMY_ON_SIX                 0x000001F8L

//
// MessageId: AIC_GET_ENEMY_OFF_US
//
// MessageText:
//
//  Get this guy off of us!%0
//
#define AIC_GET_ENEMY_OFF_US             0x000001F9L

//
// MessageId: AIC_SHAKE_THIS_GUY
//
// MessageText:
//
//  Shake this guy!%0
//
#define AIC_SHAKE_THIS_GUY               0x000001FAL

//
// MessageId: AIC_GET_GUY_OFF_ASS
//
// MessageText:
//
//  C'mon, get this guy off our ass!%0
//
#define AIC_GET_GUY_OFF_ASS              0x000001FBL

//
// MessageId: AIC_ENEMY_GOOD
//
// MessageText:
//
//  This guy is good!%0
//
#define AIC_ENEMY_GOOD                   0x000001FCL

//
// MessageId: AIC_ENEMY_GUNS_JINK
//
// MessageText:
//
//  He's firing cannon - Jink now!%0
//
#define AIC_ENEMY_GUNS_JINK              0x000001FDL

//
// MessageId: AIC_ENEMY_MISSILE_OFF
//
// MessageText:
//
//  He's got a missile off heading our way!%0
//
#define AIC_ENEMY_MISSILE_OFF            0x000001FEL

//
// MessageId: AIC_ENEMY_VERTICLE
//
// MessageText:
//
//  He's going vertical!%0
//
#define AIC_ENEMY_VERTICLE               0x000001FFL

//
// MessageId: AIC_ENEMY_WEEDS
//
// MessageText:
//
//  He's heading for the weeds!%0
//
#define AIC_ENEMY_WEEDS                  0x00000200L

//
// %1 Cardinal direction
//
// MessageId: AIC_ENEMY_DISENGAGE
//
// MessageText:
//
//  He's disengaging to the %1%0
//
#define AIC_ENEMY_DISENGAGE              0x00000201L

//
// %1 Clock position
//
// MessageId: AIC_INCOMING_MISSILE
//
// MessageText:
//
//  Incoming missile, %1%0
//
#define AIC_INCOMING_MISSILE             0x00000202L

//
// %1 Clock position
//
// MessageId: AIC_MISSILE_INBOUND
//
// MessageText:
//
//  Missile inbound, %1%0
//
#define AIC_MISSILE_INBOUND              0x00000203L

//
// %1 Clock position
//
// MessageId: AIC_HEADS_UP_MISSILE
//
// MessageText:
//
//  Heads up!  Missile Launch, %1!%0
//
#define AIC_HEADS_UP_MISSILE             0x00000204L

//
// %1 miles to target.
//
// MessageId: AIC_WSO_TARGET_DIST
//
// MessageText:
//
//  %1 miles to target.%0
//
#define AIC_WSO_TARGET_DIST              0x00000205L

//
// MessageId: AIC_MILE_TO_TARGET
//
// MessageText:
//
//  One mile to target%0
//
#define AIC_MILE_TO_TARGET               0x00000206L

//
// %1 seconds
//
// MessageId: AIC_WSO_PULL_TIME
//
// MessageText:
//
//  %1 seconds until pull%0
//
#define AIC_WSO_PULL_TIME                0x00000207L

//
// MessageId: AIC_WSO_PULL_NOW
//
// MessageText:
//
//  Pull now!%0
//
#define AIC_WSO_PULL_NOW                 0x00000208L

//
// %1 seconds
//
// MessageId: AIC_WSO_RELEASE_TIME
//
// MessageText:
//
//  %1 seconds to release%0
//
#define AIC_WSO_RELEASE_TIME             0x00000209L

//
// MessageId: AIC_WSO_WATCH_STEERING
//
// MessageText:
//
//  You're sloppy, watch your steering.%0
//
#define AIC_WSO_WATCH_STEERING           0x0000020AL

//
// MessageId: AIC_WSO_TAKE_STEERING
//
// MessageText:
//
//  Take your steering%0
//
#define AIC_WSO_TAKE_STEERING            0x0000020BL

//
// MessageId: AIC_WSO_CENTER_STEERING
//
// MessageText:
//
//  Center your steering%0
//
#define AIC_WSO_CENTER_STEERING          0x0000020CL

//
// MessageId: AIC_WSO_LOOKING_GOOD
//
// MessageText:
//
//  Looking good%0
//
#define AIC_WSO_LOOKING_GOOD             0x0000020DL

//
// MessageId: AIC_WSO_LASER_ON
//
// MessageText:
//
//  Laser on%0
//
#define AIC_WSO_LASER_ON                 0x0000020EL

//
// MessageId: AIC_WSO_LASER_OFF
//
// MessageText:
//
//  Laser off%0
//
#define AIC_WSO_LASER_OFF                0x0000020FL

//
// MessageId: AIC_WSO_LOOSING_LASE
//
// MessageText:
//
//  Losing lase, watch your steering.%0
//
#define AIC_WSO_LOOSING_LASE             0x00000210L

//
// MessageId: AIC_WSO_POD_MASKED
//
// MessageText:
//
//  The pod is masked, lost designation.%0
//
#define AIC_WSO_POD_MASKED               0x00000211L

//
// MessageId: AIC_WSO_BOMBS_GONE
//
// MessageText:
//
//  Bombs are gone.%0
//
#define AIC_WSO_BOMBS_GONE               0x00000212L

//
// MessageId: AIC_WSO_RELEASE
//
// MessageText:
//
//  Release%0
//
#define AIC_WSO_RELEASE                  0x00000213L

//
// MessageId: AIC_WSO_GOOD_RELEASE
//
// MessageText:
//
//  Bombs are gone, good release.%0
//
#define AIC_WSO_GOOD_RELEASE             0x00000214L

//
// MessageId: AIC_WSO_PICKLE
//
// MessageText:
//
//  Pickle%0
//
#define AIC_WSO_PICKLE                   0x00000215L

//
// %1 seconds
//
// MessageId: AIC_WSO_TIME_TO_IMPACT
//
// MessageText:
//
//  %1 seconds to impact.%0
//
#define AIC_WSO_TIME_TO_IMPACT           0x00000216L

//
// MessageId: AIC_WSO_IMPACT
//
// MessageText:
//
//  Impact!%0
//
#define AIC_WSO_IMPACT                   0x00000217L

//
// MessageId: AIC_WSO_GOOD_HIT
//
// MessageText:
//
//  Good hit!%0
//
#define AIC_WSO_GOOD_HIT                 0x00000218L

//
// MessageId: AIC_WSO_SHACK
//
// MessageText:
//
//  Shack!%0
//
#define AIC_WSO_SHACK                    0x00000219L

//
// MessageId: AIC_WSO_BOOM
//
// MessageText:
//
//  Boom!  Oh yes!%0
//
#define AIC_WSO_BOOM                     0x0000021AL

//
// MessageId: AIC_WSO_MISSED_DAMN
//
// MessageText:
//
//  Missed, damn it!%0
//
#define AIC_WSO_MISSED_DAMN              0x0000021BL

//
// MessageId: AIC_WSO_LOOKS_MISSED
//
// MessageText:
//
//  Looks like the bombs missed%0
//
#define AIC_WSO_LOOKS_MISSED             0x0000021CL

//
// MessageId: AIC_WSO_MISSED_SUCKS
//
// MessageText:
//
//  That sucks, we missed.%0
//
#define AIC_WSO_MISSED_SUCKS             0x0000021DL

//
// MessageId: AIC_WSO_LOOK_SECONDARIES
//
// MessageText:
//
//  Look at those secondaries!%0
//
#define AIC_WSO_LOOK_SECONDARIES         0x0000021EL

//
// MessageId: AIC_WSO_GOT_SECONDARIES
//
// MessageText:
//
//  We've got secondaries!%0
//
#define AIC_WSO_GOT_SECONDARIES          0x0000021FL

//
// MessageId: AIC_WSO_BURN
//
// MessageText:
//
//  Burn baby, burn!%0
//
#define AIC_WSO_BURN                     0x00000220L

//
// MessageId: AIC_WSO_BOOM_BOOM
//
// MessageText:
//
//  Boom Boom!  Out go the lights!%0
//
#define AIC_WSO_BOOM_BOOM                0x00000221L

//
// MessageId: AIC_WSO_DESTROYED_DALLAS
//
// MessageText:
//
//  We destroyed that bigger than Dallas!%0
//
#define AIC_WSO_DESTROYED_DALLAS         0x00000222L

//
// MessageId: AIC_WSO_WOW_LOOK
//
// MessageText:
//
//  Wow!  Look at that!%0
//
#define AIC_WSO_WOW_LOOK                 0x00000223L

//
// MessageId: AIC_WSO_SPLASH_BANDIT
//
// MessageText:
//
//  Splash our bandit!%0
//
#define AIC_WSO_SPLASH_BANDIT            0x00000224L

//
// MessageId: AIC_WSO_KILL
//
// MessageText:
//
//  That's a kill!%0
//
#define AIC_WSO_KILL                     0x00000225L

//
// MessageId: AIC_WSO_SPLASH_HELO
//
// MessageText:
//
//  Splash one helo!%0
//
#define AIC_WSO_SPLASH_HELO              0x00000226L

//
// MessageId: AIC_WSO_GOT_HIM
//
// MessageText:
//
//  We got him!%0
//
#define AIC_WSO_GOT_HIM                  0x00000227L

//
// MessageId: AIC_WSO_HISTORY
//
// MessageText:
//
//  That guy is history!%0
//
#define AIC_WSO_HISTORY                  0x00000228L

//
// MessageId: AIC_WSO_SPLASH
//
// MessageText:
//
//  Splash!%0
//
#define AIC_WSO_SPLASH                   0x00000229L

//
// MessageId: AIC_WSO_LEAD_COPIES
//
// MessageText:
//
//  Lead copies%0
//
#define AIC_WSO_LEAD_COPIES              0x0000022AL

//
// %1 AWACS callsign
//
// %2 wignman callwign
//
// MessageId: AIC_WSO_SAR_CALL
//
// MessageText:
//
//  %1, %2 has ejected, request immediate SAR scramble.%0
//
#define AIC_WSO_SAR_CALL                 0x0000022BL

//
// %1 Flight callsign
//
// MessageId: AIC_WSO_CHECK_CALL
//
// MessageText:
//
//  %1 check%0
//
#define AIC_WSO_CHECK_CALL               0x0000022CL

//
// %1 Flight callsign
//
// MessageId: AIC_WSO_STORES_CHECK
//
// MessageText:
//
//  %1 stores check.%0
//
#define AIC_WSO_STORES_CHECK             0x0000022DL

//
// %1 Flight callsign
//
// MessageId: AIC_WSO_SHOWTIME
//
// MessageText:
//
//  %1 showtime, green `em up.%0
//
#define AIC_WSO_SHOWTIME                 0x0000022EL

//
// %1 Flight callsign
//
// MessageId: AIC_WSO_CLEARED_HOT
//
// MessageText:
//
//  %1 cleared hot%0
//
#define AIC_WSO_CLEARED_HOT              0x0000022FL

//
// %1 Flight callsign
//
// MessageId: AIC_WSO_COMMENCE_ATTACK
//
// MessageText:
//
//  %1 commence attack%0
//
#define AIC_WSO_COMMENCE_ATTACK          0x00000230L

//
// %1 Tower callsign
//
// %2 Flight callsign
//
// MessageId: AIC_WSO_REQUEST_TAKEOFF
//
// MessageText:
//
//  %1, %2 flight request takeoff clearance%0
//
#define AIC_WSO_REQUEST_TAKEOFF          0x00000231L

//
// %1 Tower callsign
//
// %2 Flight callsign
//
// MessageId: AIC_WSO_REQUEST_LANDING
//
// MessageText:
//
//  %1, %2 flight request landing clearance%0
//
#define AIC_WSO_REQUEST_LANDING          0x00000232L

//
// %1 Flight callsign
//
// MessageId: AIC_WSO_RUN_EM
//
// MessageText:
//
//  %1 flight, run 'em up%0
//
#define AIC_WSO_RUN_EM                   0x00000233L

//
// MessageId: AIC_WSO_NEW_STEERING_READY
//
// MessageText:
//
//  Sequence points updated, new steering ready%0
//
#define AIC_WSO_NEW_STEERING_READY       0x00000234L

//
// MessageId: AIC_WSO_FAC_MARKED
//
// MessageText:
//
//  FAC location marked in the system%0
//
#define AIC_WSO_FAC_MARKED               0x00000235L

//
// %1 bomber callsign
//
// %2 Player callsign
//
// MessageId: AIC_WSO_FORM_BOMBER
//
// MessageText:
//
//  %1, %2, we have you in sight, forming up.%0
//
#define AIC_WSO_FORM_BOMBER              0x00000236L

//
// %1 Bomber callsign
//
// %2 Player callsign
//
// MessageId: AIC_WSO_ON_WAY
//
// MessageText:
//
//  %1, %2, on our way.%0
//
#define AIC_WSO_ON_WAY                   0x00000237L

//
// %1 Bomber callsign
//
// %2 Player callsign
//
// MessageId: AIC_WSO_WITH_YOU
//
// MessageText:
//
//  %1, %2, we're with you.%0
//
#define AIC_WSO_WITH_YOU                 0x00000238L

//
// MessageId: AIC_WSO_WHAT_THE_HELL
//
// MessageText:
//
//  What the hell are you doing?%0
//
#define AIC_WSO_WHAT_THE_HELL            0x00000239L

//
// MessageId: AIC_WSO_THAT_WAS_SMART
//
// MessageText:
//
//  Oh, that was smart!%0
//
#define AIC_WSO_THAT_WAS_SMART           0x0000023AL

//
// MessageId: AIC_WSO_WHAT_HELL_WRONG
//
// MessageText:
//
//  What the hell is wrong with you!%0
//
#define AIC_WSO_WHAT_HELL_WRONG          0x0000023BL

//
// MessageId: AIC_WSO_THAT_WAS_STUPID
//
// MessageText:
//
//  That was STUPID!%0
//
#define AIC_WSO_THAT_WAS_STUPID          0x0000023CL

//
// %1 wing callsign
//
// %2 Player callsign
//
// MessageId: AIC_WSO_HELP_ON_WAY
//
// MessageText:
//
//  %1, %2, we hear you, help is on the way%0
//
#define AIC_WSO_HELP_ON_WAY              0x0000023DL

//
// %1 wing callsign
//
// %2 Player callsign
//
// MessageId: AIC_WSO_HANG_IN_THERE
//
// MessageText:
//
//  %1, %2, copy hang in there%0
//
#define AIC_WSO_HANG_IN_THERE            0x0000023EL

//
// %1 wing callsign
//
// %2 Player callsign
//
// MessageId: AIC_WSO_SANDY_INBOUND
//
// MessageText:
//
//  %1, %2, Sandy inbound%0
//
#define AIC_WSO_SANDY_INBOUND            0x0000023FL

//
// %1 JSTARS callsign
//
// %2 flight callsign
//
// MessageId: AIC_WSO_J_CHECK_IN
//
// MessageText:
//
//  %1, %2, checking in%0
//
#define AIC_WSO_J_CHECK_IN               0x00000240L

//
// %1 JSTARS callsign
//
// %2 flight callsign
//
// MessageId: AIC_WSO_J_AVAILABLE
//
// MessageText:
//
//  %1, %2, available for tasking%0
//
#define AIC_WSO_J_AVAILABLE              0x00000241L

//
// %1 JSTARS callsign
//
// %2 flight callsign
//
// MessageId: AIC_WSO_J_REQUEST_TARGET
//
// MessageText:
//
//  %1, %2, request new target%0
//
#define AIC_WSO_J_REQUEST_TARGET         0x00000242L

//
// %1 JSTARS callsign
//
// %2 flight callsign
//
// MessageId: AIC_WSO_J_HEADING_HOME
//
// MessageText:
//
//  %1, %2, we are heading home%0
//
#define AIC_WSO_J_HEADING_HOME           0x00000243L

//
// %1 JSTARS callsign
//
// %2 flight callsign
//
// MessageId: AIC_WSO_J_RTB
//
// MessageText:
//
//  %1, %2, returning to base %0
//
#define AIC_WSO_J_RTB                    0x00000244L

//
// %1 minutes to nav point
//
// MessageId: AIC_WSO_MINUTES_TO_NAV
//
// MessageText:
//
//  %1 minutes to nav point%0
//
#define AIC_WSO_MINUTES_TO_NAV           0x00000245L

//
// MessageId: AIC_WSO_RADAR_BIT_CLEAN
//
// MessageText:
//
//  Radar BIT runs clean%0
//
#define AIC_WSO_RADAR_BIT_CLEAN          0x00000246L

//
// MessageId: AIC_WSO_WINDERS_OK
//
// MessageText:
//
//  Winders checkout ok%0
//
#define AIC_WSO_WINDERS_OK               0x00000247L

//
// MessageId: AIC_WSO_SPARROWS_GOOD
//
// MessageText:
//
//  Sparrows check good%0
//
#define AIC_WSO_SPARROWS_GOOD            0x00000248L

//
// MessageId: AIC_WSO_SLAMMERS_GOOD
//
// MessageText:
//
//  Slammers look good%0
//
#define AIC_WSO_SLAMMERS_GOOD            0x00000249L

//
// MessageId: AIC_WSO_A2G_OK
//
// MessageText:
//
//  Air to Mud stores look ok%0
//
#define AIC_WSO_A2G_OK                   0x0000024AL

//
// MessageId: AIC_WSO_IFF_SET
//
// MessageText:
//
//  IFF set%0
//
#define AIC_WSO_IFF_SET                  0x0000024BL

//
// MessageId: AIC_WSO_ENGINES_GOOD
//
// MessageText:
//
//  Engines look good%0
//
#define AIC_WSO_ENGINES_GOOD             0x0000024CL

//
// MessageId: AIC_WSO_TEWS_UP
//
// MessageText:
//
//  TEWS powered up%0
//
#define AIC_WSO_TEWS_UP                  0x0000024DL

//
// MessageId: AIC_WSO_RADIO_SET
//
// MessageText:
//
//  Radios set%0
//
#define AIC_WSO_RADIO_SET                0x0000024EL

//
// MessageId: AIC_WSO_CHECK_LIGHTS_OFF
//
// MessageText:
//
//  Check external lights off%0
//
#define AIC_WSO_CHECK_LIGHTS_OFF         0x0000024FL

//
// MessageId: AIC_WSO_PLATFORM_TIGHT
//
// MessageText:
//
//  Platform looks tight%0
//
#define AIC_WSO_PLATFORM_TIGHT           0x00000250L

//  
//
// MessageId: AIC_BREAK_RIGHT_2
//
// MessageText:
//
//  break right%0
//
#define AIC_BREAK_RIGHT_2                0x00000251L

//  
//
// MessageId: AIC_BREAK_LEFT_2
//
// MessageText:
//
//  break left%0
//
#define AIC_BREAK_LEFT_2                 0x00000252L

//  
//
// MessageId: AIC_BREAK_HIGH_2
//
// MessageText:
//
//  break high%0
//
#define AIC_BREAK_HIGH_2                 0x00000253L

//  
//
// MessageId: AIC_BREAK_LOW_2
//
// MessageText:
//
//  break low%0
//
#define AIC_BREAK_LOW_2                  0x00000254L

//
// MessageId: AIC_WSO_CHANGE_RADIO
//
// MessageText:
//
//  Pushing UHF two to new freq.%0
//
#define AIC_WSO_CHANGE_RADIO             0x00000255L

//
// MessageId: AIC_WSO_MISSION_COMPLETE
//
// MessageText:
//
//  Mission complete%0
//
#define AIC_WSO_MISSION_COMPLETE         0x00000256L

//
// MessageId: AIC_WSO_COMPLETE_HOME
//
// MessageText:
//
//  Mission complete, let's go home%0
//
#define AIC_WSO_COMPLETE_HOME            0x00000257L

//
// MessageId: AIC_WSO_TIME_GO_HOME
//
// MessageText:
//
//  Time to go home.%0
//
#define AIC_WSO_TIME_GO_HOME             0x00000258L

//
// %1 Aircraft callsign
//
// MessageId: AIC_THIS_IS
//
// MessageText:
//
//  This is %1%0
//
#define AIC_THIS_IS                      0x00000259L

//
// MessageId: AIC_AWACS_PICT_CLEAN
//
// MessageText:
//
//  Picture clean%0
//
#define AIC_AWACS_PICT_CLEAN             0x0000025AL

//
// MessageId: AIC_AWACS_PICT_UNCHANGED
//
// MessageText:
//
//  Picture unchanged%0
//
#define AIC_AWACS_PICT_UNCHANGED         0x0000025BL

//
// MessageId: AIC_AWACS_PICT_SAME
//
// MessageText:
//
//  Picture same%0
//
#define AIC_AWACS_PICT_SAME              0x0000025CL

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_CLEAN
//
// MessageText:
//
//  %1, %2, clean%0
//
#define AIC_AWACS_CLEAN                  0x0000025DL

//
// %1 AWACS callsign
//
// %2 speed (fast/slow mover)
//
// %3 size (group/single)
//
// %4 cardinal direction
//
// %5 range (# miles)
//
// %6 altitude (high/med/low)
//
// MessageId: AIC_AWACS_BULLSEYE_PICT
//
// MessageText:
//
//  %1, %2, %3, %4 bullseye, %5, %6%0
//
#define AIC_AWACS_BULLSEYE_PICT          0x0000025EL

//
// %1 fighter callsign
//
// %2 AWACS callsign
//
// %3 speed (fast/slow mover)
//
// %4 size (group/single)
//
// %5 bearing in degrees
//
// %6 range (# miles)
//
// %7 altitude (high/med/low)
//
// MessageId: AIC_AWACS_BEARING_PICT
//
// MessageText:
//
//  %1, %2, %3 %4 bearing %5, %6, %7%0
//
#define AIC_AWACS_BEARING_PICT           0x0000025FL

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// %3 speed (fast/slow mover)
//
// %4 <group/single>
//
// %5 bearing
//
// %6 range
//
// %7 altitude
//
// %8 aspect
//
// MessageId: AIC_AWACS_BEARING
//
// MessageText:
//
//  %1, %2, %3 %4 bearing %5, %6, %7, %8.%0
//
#define AIC_AWACS_BEARING                0x00000260L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// %3 speed (fast/slow mover)
//
// %4 bearing
//
// %5 range
//
// %6 altitude
//
// %7 aspect
//
// MessageId: AIC_AWACS_THREAT_BEARING
//
// MessageText:
//
//  %1, %2,  %3 threat bearing %4, %5, %6, %7.%0
//
#define AIC_AWACS_THREAT_BEARING         0x00000261L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// %3 bearing
//
// %4 range
//
// %5 altitude
//
// %6 aspect
//
// MessageId: AIC_AWACS_FRIENDLY_BEARING
//
// MessageText:
//
//  %1, %2, friendly traffic bearing %3, %4, %5, %6.%0
//
#define AIC_AWACS_FRIENDLY_BEARING       0x00000262L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// %3 bearing
//
// %4 range
//
// MessageId: AIC_AWACS_NEW_THREAT_BEARING
//
// MessageText:
//
//  %1, %2, Be advised, new threats airborne at bearing %3, %4%0
//
#define AIC_AWACS_NEW_THREAT_BEARING     0x00000263L

//
// %1 AWACS callsign
//
// %2 cardinal direction
//
// %3 range
//
// MessageId: AIC_AWACS_NEW_THREAT_BULLSEYE
//
// MessageText:
//
//  %1, Be advised, new threats airborne at %2 bullseye, %3%0
//
#define AIC_AWACS_NEW_THREAT_BULLSEYE    0x00000264L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_MERGED
//
// MessageText:
//
//  %1, %2, merged.%0
//
#define AIC_AWACS_MERGED                 0x00000265L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_COPY
//
// MessageText:
//
//  %1, %2, copy%0
//
#define AIC_AWACS_COPY                   0x00000266L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_ACKNOWLEDGED
//
// MessageText:
//
//  %1, %2, acknowledged.%0
//
#define AIC_AWACS_ACKNOWLEDGED           0x00000267L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_SAR_ACK
//
// MessageText:
//
//  %1, %2 copy, SAR package enroute.%0
//
#define AIC_AWACS_SAR_ACK                0x00000268L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_NEGATIVE
//
// MessageText:
//
//  %1, %2, negative.%0
//
#define AIC_AWACS_NEGATIVE               0x00000269L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_VECTORING_FIGHTERS
//
// MessageText:
//
//  %1, %2, copy, vectoring fighters to your location.%0
//
#define AIC_AWACS_VECTORING_FIGHTERS     0x0000026AL

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_NO_FIGHTERS
//
// MessageText:
//
//  %1, %2, negative, no assets available.%0
//
#define AIC_AWACS_NO_FIGHTERS            0x0000026BL

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_WEASELS
//
// MessageText:
//
//  %1, %2, copy, Weasel support inbound.%0
//
#define AIC_AWACS_WEASELS                0x0000026CL

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_NO_WEASELS
//
// MessageText:
//
//  %1, %2, negative, no Weasels available.%0
//
#define AIC_AWACS_NO_WEASELS             0x0000026DL

//
// %1 <This is> AWACS callsign
//
// MessageId: AIC_AWACS_MISSILES_INBOUND
//
// MessageText:
//
//  %1, Missiles inbound!%0
//
#define AIC_AWACS_MISSILES_INBOUND       0x0000026EL

//
// %1 <This is> AWACS callsign
//
// MessageId: AIC_AWACS_RUNNING
//
// MessageText:
//
//  %1 Shutting down and running from bandits!%0
//
#define AIC_AWACS_RUNNING                0x0000026FL

//
// %1 <This is> AWACS callsign
//
// MessageId: AIC_AWACS_GOING_IN
//
// MessageText:
//
//  %1, we've been hit, going in!%0
//
#define AIC_AWACS_GOING_IN               0x00000270L

//
// %1 <This is> AWACS callsign
//
// MessageId: AIC_AWACS_MAYDAY
//
// MessageText:
//
//  %1, MAYDAY MAYDAY!%0
//
#define AIC_AWACS_MAYDAY                 0x00000271L

//
// %1 <This is> AWACS callsign
//
// MessageId: AIC_AWACS_HIT_SEND_HELP
//
// MessageText:
//
//  %1, we're hit, send help!%0
//
#define AIC_AWACS_HIT_SEND_HELP          0x00000272L

//
// MessageId: AIC_FAST_MOVING
//
// MessageText:
//
//  Fast moving%0
//
#define AIC_FAST_MOVING                  0x00000273L

//
// MessageId: AIC_SLOW_MOVING
//
// MessageText:
//
//  Slow moving%0
//
#define AIC_SLOW_MOVING                  0x00000274L

//
// %1 Cardinal direction
//
// MessageId: AIC_BULLSEYE_MSG
//
// MessageText:
//
//  %1 bullseye%0
//
#define AIC_BULLSEYE_MSG                 0x00000275L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_STANDBY
//
// MessageText:
//
//  %1, %2, We have priority tasking for you.  Standby for words.%0
//
#define AIC_AWACS_STANDBY                0x00000276L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_SUPPORT_GROUND
//
// MessageText:
//
//  %1, %2, Vector immediately to new coordinates to support ground forces.%0
//
#define AIC_AWACS_SUPPORT_GROUND         0x00000277L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_NEW_COORDINATES
//
// MessageText:
//
//  %1, %2, transmitting new coordinates on dolly.%0
//
#define AIC_AWACS_NEW_COORDINATES        0x00000278L

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// MessageId: AIC_AWACS_GROUND_FAC
//
// MessageText:
//
//  %1, %2, upon arrival at new coordinates hold and orbit.  Ground FAC will contact you.%0
//
#define AIC_AWACS_GROUND_FAC             0x00000279L

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// MessageId: AIC_JSTARS_SPECIAL
//
// MessageText:
//
//  %1, %2, standy for special tasking.%0
//
#define AIC_JSTARS_SPECIAL               0x0000027AL

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// MessageId: AIC_JSTARS_COPY_STANDBY
//
// MessageText:
//
//  %1, %2, copy, standby.%0
//
#define AIC_JSTARS_COPY_STANDBY          0x0000027BL

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// MessageId: AIC_JSTARS_COPY_CLEARED_PATROL
//
// MessageText:
//
//  %1, %2, copy, cleared to patrol as fragged.%0
//
#define AIC_JSTARS_COPY_CLEARED_PATROL   0x0000027CL

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// MessageId: AIC_JSTARS_COPY_GOOD_HUNT
//
// MessageText:
//
//  %1, %2, copy, good hunting.%0
//
#define AIC_JSTARS_COPY_GOOD_HUNT        0x0000027DL

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// MessageId: AIC_JSTARS_POSSIBLE_TARG
//
// MessageText:
//
//  %1, %2, copy, possible target located, standby for words.%0
//
#define AIC_JSTARS_POSSIBLE_TARG         0x0000027EL

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// %3 bullseye
//
// %4 range
//
// MessageId: AIC_JSTARS_GROUND_AT
//
// MessageText:
//
//  %1, %2, ground targets located at %3 bullseye, %4%0
//
#define AIC_JSTARS_GROUND_AT             0x0000027FL

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// %3 bullseye
//
// %4 range
//
// MessageId: AIC_JSTARS_MOVERS_AT
//
// MessageText:
//
//  %1, %2, movers at %3 bullseye, %4.%0
//
#define AIC_JSTARS_MOVERS_AT             0x00000280L

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// %3 bullseye
//
// %4 range
//
// MessageId: AIC_JSTARS_VEHICLES_AT
//
// MessageText:
//
//  %1, %2, vehicle traffic at %3 bullseye, %4.%0
//
#define AIC_JSTARS_VEHICLES_AT           0x00000281L

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// %3 bearing
//
// %4 range
//
// MessageId: AIC_JSTARS_GROUND_BEARING
//
// MessageText:
//
//  %1, %2, ground targets bearing %3, %4.%0
//
#define AIC_JSTARS_GROUND_BEARING        0x00000282L

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// %3 bearing
//
// %4 range
//
// MessageId: AIC_JSTARS_MOVERS_BEARING
//
// MessageText:
//
//  %1, %2, movers bearing %3, %4.%0
//
#define AIC_JSTARS_MOVERS_BEARING        0x00000283L

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// %3 bearing
//
// %4 range
//
// MessageId: AIC_JSTARS_VEHCILES_BEARING
//
// MessageText:
//
//  %1, %2, vehicle traffic bearing %3, %4.%0
//
#define AIC_JSTARS_VEHCILES_BEARING      0x00000284L

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// MessageId: AIC_JSTARS_AREA_CLEAR
//
// MessageText:
//
//  %1, %2, area appears clear.%0
//
#define AIC_JSTARS_AREA_CLEAR            0x00000285L

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// MessageId: AIC_JSTARS_UNSURE
//
// MessageText:
//
//  %1, %2, ambiguous returns, standby.%0
//
#define AIC_JSTARS_UNSURE                0x00000286L

//
// %1 Player callsign
//
// %2 JSTARS callsign
//
// MessageId: AIC_JSTARS_BROKEN
//
// MessageText:
//
//  %1, %2, gadget bent, you are on your own.%0
//
#define AIC_JSTARS_BROKEN                0x00000287L

//
// %1 <This is> JSTARS callsign
//
// MessageId: AIC_JSTARS_UNDER_ATTACK
//
// MessageText:
//
//  %1, we are under attack!%0
//
#define AIC_JSTARS_UNDER_ATTACK          0x00000288L

//
// %1 <This is> JSTARS callsign
//
// MessageId: AIC_JSTARS_BANDITS_CLOSING
//
// MessageText:
//
//  %1, bandits closing on us!%0
//
#define AIC_JSTARS_BANDITS_CLOSING       0x00000289L

//
// %1 <This is> JSTARS callsign
//
// MessageId: AIC_JSTARS_GOING_DOWN
//
// MessageText:
//
//  %1, we've been hit, going down!%0
//
#define AIC_JSTARS_GOING_DOWN            0x0000028AL

//
// %1 <This is> JSTARS callsign
//
// MessageId: AIC_JSTARS_MAYDAY
//
// MessageText:
//
//  %1, MAYDAY, MAYDAY!%0
//
#define AIC_JSTARS_MAYDAY                0x0000028BL

//
// %1 <This is> JSTARS callsign
//
// MessageId: AIC_JSTARS_SEND_HELP
//
// MessageText:
//
//  %1, we're hit, send help!%0
//
#define AIC_JSTARS_SEND_HELP             0x0000028CL

//
// %1 Player callsign
//
// %2 Tower callsign
//
// %3 winds string
//
// %4 ceiling string
//
// %5 visablitly
//
// MessageId: AIC_TOWER_CLEAR_TAKEOFF
//
// MessageText:
//
//  %1 one, %2, %3 %4 %5 cleared for takeoff, good luck%0
//
#define AIC_TOWER_CLEAR_TAKEOFF          0x0000028DL

//
// %1 Player callsign
//
// %2 Tower callsign
//
// %3 winds string
//
// %4 ceiling string
//
// %5 visablitly
//
// %6 AWACS call sign
//
// MessageId: AIC_TOWER_TAKEOFF_CONTACT
//
// MessageText:
//
//  %1 one, %2, %3 %4 %5 cleared for takeoff, contact %6 for airborne control.%0
//
#define AIC_TOWER_TAKEOFF_CONTACT        0x0000028EL

//
// %1 Player callsign
//
// %2 Tower callsign
//
// MessageId: AIC_TOWER_HOLD_SHORT
//
// MessageText:
//
//  %1 one, %2, hold short for landing traffic.%0
//
#define AIC_TOWER_HOLD_SHORT             0x0000028FL

//
// %1 Player callsign
//
// %2 Tower callsign
//
// MessageId: AIC_TOWER_HOLD_IFR
//
// MessageText:
//
//  %1 one, %2, Taxi into position and hold, awaiting IFR release.%0
//
#define AIC_TOWER_HOLD_IFR               0x00000290L

//
// %1 Player callsign
//
// %2 Tower callsign
//
// MessageId: AIC_TOWER_CLEAR_LAND
//
// MessageText:
//
//  %1 one, %2, cleared for landing.%0
//
#define AIC_TOWER_CLEAR_LAND             0x00000291L

//
// %1 Player callsign
//
// %2 Tower callsign
//
// MessageId: AIC_TOWER_HOLD_TRAFFIC
//
// MessageText:
//
//  %1, %2, hold for traffic.%0
//
#define AIC_TOWER_HOLD_TRAFFIC           0x00000292L

//
// %1 Player callsign
//
// %2 Tower callsign
//
// %3!d! Altitude (in 1000s of feet) as number.
//
// MessageId: AIC_TOWER_HOLD_ALTITUDE
//
// MessageText:
//
//  %1, %2, hold for traffic at Angels %3!d!.%0
//
#define AIC_TOWER_HOLD_ALTITUDE          0x00000293L

//
// %1 Player callsign
//
// %2 Tower callsign
//
// %3 cardinal direction
//
// MessageId: AIC_TOWER_SCRAMBLE
//
// MessageText:
//
//  %1 one, %2, scramble, scramble, bogies inbound from the %3.%0
//
#define AIC_TOWER_SCRAMBLE               0x00000294L

//
// %1 wind direction
//
// %2 wind speed
//
// MessageId: AIC_TOWER_WINDS_ARE
//
// MessageText:
//
//  winds are %1 at %2%0
//
#define AIC_TOWER_WINDS_ARE              0x00000295L

//
// %1 height in 1000s of ft
//
// MessageId: AIC_TOWER_CEILING
//
// MessageText:
//
//  ceiling angels %1%0
//
#define AIC_TOWER_CEILING                0x00000296L

//
// MessageId: AIC_TOWER_CEILING_UNLIMITED
//
// MessageText:
//
//  ceiling unlimited%0
//
#define AIC_TOWER_CEILING_UNLIMITED      0x00000297L

//
// MessageId: AIC_TOWER_VIS_HALF_MILE
//
// MessageText:
//
//  visibility one half mile%0
//
#define AIC_TOWER_VIS_HALF_MILE          0x00000298L

//
// MessageId: AIC_TOWER_VIS_ONE_MILE
//
// MessageText:
//
//  visibility one mile%0
//
#define AIC_TOWER_VIS_ONE_MILE           0x00000299L

//
// %1 miles of visibility
//
// MessageId: AIC_TOWER_VISABILITY
//
// MessageText:
//
//  visibility %1 miles%0
//
#define AIC_TOWER_VISABILITY             0x0000029AL

//
// MessageId: AIC_TOWER_VIS_UNLIMITED
//
// MessageText:
//
//  visibility unlimited%0
//
#define AIC_TOWER_VIS_UNLIMITED          0x0000029BL

//
// MessageId: AIC_TOWER_WINDS_CALM
//
// MessageText:
//
//  calm%0
//
#define AIC_TOWER_WINDS_CALM             0x0000029CL

//
// %1 hundreds of degrees
//
// %2 bearing in tens of degrees
//
// MessageId: AIC_TOWER_WIND_DIR
//
// MessageText:
//
//  %1 %2 at%0
//
#define AIC_TOWER_WIND_DIR               0x0000029DL

//
// %1 wind speed in 5 nm inc.
//
// MessageId: AIC_TOWER_WIND_SPEED
//
// MessageText:
//
//  %1%0
//
#define AIC_TOWER_WIND_SPEED             0x0000029EL

//
// %1 Player callsign
//
// %2 Tower callsign
//
// %3 bearing
//
// %4 range
//
// %5 altitude
//
// MessageId: AIC_TOWER_FRIENDLY_TRAFFIC
//
// MessageText:
//
//  %1 one, %2, friendly traffic bearing, %3, %4, %5.%0
//
#define AIC_TOWER_FRIENDLY_TRAFFIC       0x0000029FL

//
// %1 height in 1000s of ft
//
// MessageId: AIC_ANGELS
//
// MessageText:
//
//  angels %1%0
//
#define AIC_ANGELS                       0x000002A0L

//
// %1 AWACS callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_ON_STATION
//
// MessageText:
//
//  %1, %2, on station.%0
//
#define AIC_CAP_ON_STATION               0x000002A1L

//
// %1 Player callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_BEGINNING_SWEEP
//
// MessageText:
//
//  %1, %2, we're beginning our sweep of the target area now%0
//
#define AIC_CAP_BEGINNING_SWEEP          0x000002A2L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_TALLY_BANDIT
//
// MessageText:
//
//  %1,  Tally bandit%0
//
#define AIC_CAP_TALLY_BANDIT             0x000002A3L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_TALLY_BANDITS
//
// MessageText:
//
//  %1, Tally bandits%0
//
#define AIC_CAP_TALLY_BANDITS            0x000002A4L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_ENGAGING_BANDIT
//
// MessageText:
//
//  %1, Engaging bandit%0
//
#define AIC_CAP_ENGAGING_BANDIT          0x000002A5L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_ENGAGING_BANDITS
//
// MessageText:
//
//  %1, Engaging bandits%0
//
#define AIC_CAP_ENGAGING_BANDITS         0x000002A6L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_INTERCEPT_BANDIT_2
//
// MessageText:
//
//  %1, Intercepting incoming bandit%0
//
#define AIC_CAP_INTERCEPT_BANDIT_2       0x000002A7L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_INTERCEPT_BANDITS_2
//
// MessageText:
//
//  %1, Intercepting incoming bandits%0
//
#define AIC_CAP_INTERCEPT_BANDITS_2      0x000002A8L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_SPLASH_ONE
//
// MessageText:
//
//  %1, Splash One.%0
//
#define AIC_CAP_SPLASH_ONE               0x000002A9L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_BANDIT_KILLED
//
// MessageText:
//
//  %1, Bandit killed!%0
//
#define AIC_CAP_BANDIT_KILLED            0x000002AAL

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_ENGAGED_SAM
//
// MessageText:
//
//  %1 engaged defensive SAM!%0
//
#define AIC_CAP_ENGAGED_SAM              0x000002ABL

//
// %1 AWACS callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_EJECTING
//
// MessageText:
//
//  %1, %2, is hit ejecting!%0
//
#define AIC_CAP_EJECTING                 0x000002ACL

//
// %1 AWACS callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_FUEL_HOME
//
// MessageText:
//
//  %1, %2, bingo fuel, heading for home.%0
//
#define AIC_CAP_FUEL_HOME                0x000002ADL

//
// %1 AWACS callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_FUEL_TANKER
//
// MessageText:
//
//  %1, %2, bingo fuel, heading for tanker.%0
//
#define AIC_CAP_FUEL_TANKER              0x000002AEL

//
// %1 AWACS callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_WINCHESTER
//
// MessageText:
//
//  %1, %2, is Winchester at this time, returning to base.%0
//
#define AIC_CAP_WINCHESTER               0x000002AFL

//
// %1 AWACS callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_RTB
//
// MessageText:
//
//  %1, %2, you're clear, returning to base.%0
//
#define AIC_CAP_RTB                      0x000002B0L

//
// %1 Player callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_HEADED_HOME
//
// MessageText:
//
//  %1, %2, we're headed for home%0
//
#define AIC_CAP_HEADED_HOME              0x000002B1L

//
// %1 AWACS callsign
//
// %2 fighter callsign
//
// %3 speed (fast/slow mover)
//
// %4 size (group/single)
//
// %5 cardinal direction
//
// %6 range (# miles)
//
// %7 altitude (high/med/low)
//
// MessageId: AIC_CAP_CONTACT
//
// MessageText:
//
//  %1, %2, contact %3, %4, %5 bullseye, %6, %7%0
//
#define AIC_CAP_CONTACT                  0x000002B2L

//
// %1 AWACS callsign
//
// %2 fighter callsign
//
// %3 speed (fast/slow mover)
//
// %4 size (group/single)
//
// %5 bearing in degrees
//
// %6 range (# miles)
//
// %7 altitude (high/med/low)
//
// MessageId: AIC_CAP_CONTACT_BEARING
//
// MessageText:
//
//  %1, %2, %3 %4 radar contact bearing %5, %6, %7%0
//
#define AIC_CAP_CONTACT_BEARING          0x000002B3L

//
// %1 AWACS callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_CLEAN
//
// MessageText:
//
//  %1, %2, clean%0
//
#define AIC_CAP_CLEAN                    0x000002B4L

//
// %1 Player callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_FORM_PLAYER
//
// MessageText:
//
//  %1, %2, we have you in sight, forming up.%0
//
#define AIC_CAP_FORM_PLAYER              0x000002B5L

//
// %1 Player callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_ON_WAY
//
// MessageText:
//
//  %1, %2, on our way.%0
//
#define AIC_CAP_ON_WAY                   0x000002B6L

//
// %1 Player callsign
//
// %2 fighter callsign
//
// MessageId: AIC_CAP_WITH_YOU
//
// MessageText:
//
//  %1, %2, we're with you.%0
//
#define AIC_CAP_WITH_YOU                 0x000002B7L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_INBOUND
//
// MessageText:
//
//  %1, %2, we are inbound at this time.%0
//
#define AIC_SAR_INBOUND                  0x000002B8L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_ON_WAY
//
// MessageText:
//
//  %1, %2, on our way.%0
//
#define AIC_SAR_ON_WAY                   0x000002B9L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_SCRAMBLE
//
// MessageText:
//
//  %1, %2, scrambling to designated coordinates.%0
//
#define AIC_SAR_SCRAMBLE                 0x000002BAL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_MOVING_IN_LZ
//
// MessageText:
//
//  %1, %2, we are moving in to LZ at this time, over.%0
//
#define AIC_SAR_MOVING_IN_LZ             0x000002BBL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_ADVISED_MOVING_IN
//
// MessageText:
//
//  %1, %2, be advised, we are moving in, over.%0
//
#define AIC_SAR_ADVISED_MOVING_IN        0x000002BCL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_APPROACHING_LZ
//
// MessageText:
//
//  %1, %2, we are now approaching LZ, over.%0
//
#define AIC_SAR_APPROACHING_LZ           0x000002BDL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_DOWN_AT_LZ
//
// MessageText:
//
//  %1, %2, we're down at LZ, over.%0
//
#define AIC_SAR_DOWN_AT_LZ               0x000002BEL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_NOW_AT_LZ
//
// MessageText:
//
//  %1, %2, we are now at LZ, over.%0
//
#define AIC_SAR_NOW_AT_LZ                0x000002BFL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_TOUCHING_LZ
//
// MessageText:
//
//  %1, %2, touching down at LZ now, over.%0
//
#define AIC_SAR_TOUCHING_LZ              0x000002C0L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_PICKUP_OK
//
// MessageText:
//
//  %1, %2, pickup successful, returning to base.%0
//
#define AIC_SAR_PICKUP_OK                0x000002C1L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_GOT_EM
//
// MessageText:
//
//  %1, %2, we got em, heading home.%0
//
#define AIC_SAR_GOT_EM                   0x000002C2L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_NO_FIND
//
// MessageText:
//
//  %1, %2, be advised, we are unable to locate the downed crew.%0
//
#define AIC_SAR_NO_FIND                  0x000002C3L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_NO_FRIENDLIES
//
// MessageText:
//
//  %1, %2, no friendlies at LZ, returning to base.%0
//
#define AIC_SAR_NO_FRIENDLIES            0x000002C4L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_LAUNCH_LAUNCH
//
// MessageText:
//
//  %1, %2, Launch!  Launch!%0
//
#define AIC_SAR_LAUNCH_LAUNCH            0x000002C5L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_MISSILE_LAUNCH
//
// MessageText:
//
//  %1, %2, Missile Launch!%0
//
#define AIC_SAR_MISSILE_LAUNCH           0x000002C6L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_SAM_SAM
//
// MessageText:
//
//  %1, %2, SAM! SAM!%0
//
#define AIC_SAR_SAM_SAM                  0x000002C7L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_SAM_LAUNCH
//
// MessageText:
//
//  %1, %2, SAM Launch!%0
//
#define AIC_SAR_SAM_LAUNCH               0x000002C8L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_AAA_FIRE
//
// MessageText:
//
//  %1, %2, We're taking Triple-A fire!%0
//
#define AIC_SAR_AAA_FIRE                 0x000002C9L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_HEAVY_AAA
//
// MessageText:
//
//  %1, %2, encountering heavy Triple-A!%0
//
#define AIC_SAR_HEAVY_AAA                0x000002CAL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_WE_TAKING_FIRE
//
// MessageText:
//
//  %1, %2, we're taking fire!%0
//
#define AIC_SAR_WE_TAKING_FIRE           0x000002CBL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_TAKING_FIRE
//
// MessageText:
//
//  %1, %2, taking fire!%0
//
#define AIC_SAR_TAKING_FIRE              0x000002CCL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_WE_ARE_HIT
//
// MessageText:
//
//  %1, %2, we're Hit!%0
//
#define AIC_SAR_WE_ARE_HIT               0x000002CDL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_HEAVY_DAMAGE
//
// MessageText:
//
//  %1, %2, we are taking heavy damage!%0
//
#define AIC_SAR_HEAVY_DAMAGE             0x000002CEL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_NEED_COVER
//
// MessageText:
//
//  %1, %2, we are under fire, we need some cover!%0
//
#define AIC_SAR_NEED_COVER               0x000002CFL

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_NEED_HELP
//
// MessageText:
//
//  %1, %2, We're hit, need some help here!%0
//
#define AIC_SAR_NEED_HELP                0x000002D0L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_MAYDAY_DOWN
//
// MessageText:
//
//  %1, %2, Mayday, mayday, we are going down!%0
//
#define AIC_SAR_MAYDAY_DOWN              0x000002D1L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_GOING_DOWN
//
// MessageText:
//
//  %1, %2, We are going down.  I say again, we are going down.%0
//
#define AIC_SAR_GOING_DOWN               0x000002D2L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_MAYDAY
//
// MessageText:
//
//  %1, %2, Mayday!  Mayday!  We've lost control!%0
//
#define AIC_SAR_MAYDAY                   0x000002D3L

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_ON_STATION
//
// MessageText:
//
//  %1, %2, on station.%0
//
#define AIC_SEAD_ON_STATION              0x000002D4L

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// %3 cardinal direction
//
// MessageId: AIC_SEAD_MUD_SPIKE
//
// MessageText:
//
//  %1, %2, Mud spike %3.%0
//
#define AIC_SEAD_MUD_SPIKE               0x000002D5L

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// %3 cardinal direction
//
// MessageId: AIC_SEAD_BUDDY_SPIKE
//
// MessageText:
//
//  %1, %2, Buddy spike %3.%0
//
#define AIC_SEAD_BUDDY_SPIKE             0x000002D6L

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// %3 cardinal direction
//
// MessageId: AIC_SEAD_SPIKE
//
// MessageText:
//
//  %1, %2, Spike %3.%0
//
#define AIC_SEAD_SPIKE                   0x000002D7L

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_NAKED
//
// MessageText:
//
//  %1, %2, Naked%0
//
#define AIC_SEAD_NAKED                   0x000002D8L

//
// MessageId: AIC_SEAD_MAGNUM
//
// MessageText:
//
//  Magnum!%0
//
#define AIC_SEAD_MAGNUM                  0x000002D9L

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_ATTACKING_SAM
//
// MessageText:
//
//  %1, %2, attacking SAM launcher.%0
//
#define AIC_SEAD_ATTACKING_SAM           0x000002DAL

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_ATTACKING_AAA
//
// MessageText:
//
//  %1, %2, attacking Triple-A site%0
//
#define AIC_SEAD_ATTACKING_AAA           0x000002DBL

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_WINCHESTER
//
// MessageText:
//
//  %1, %2, Winchester, returning to base.%0
//
#define AIC_SEAD_WINCHESTER              0x000002DCL

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_MUSIC_ON
//
// MessageText:
//
//  %1, %2, music on.%0
//
#define AIC_SEAD_MUSIC_ON                0x000002DDL

//
// %1 AWACS callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_MUSIC_OFF
//
// MessageText:
//
//  %1, %2, music off.%0
//
#define AIC_SEAD_MUSIC_OFF               0x000002DEL

//
// %1 Player callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_FORMING_UP
//
// MessageText:
//
//  %1, %2, we have you in sight, forming up.%0
//
#define AIC_SEAD_FORMING_UP              0x000002DFL

//
// %1 Player callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_ON_WAY
//
// MessageText:
//
//  %1, %2, on our way.%0
//
#define AIC_SEAD_ON_WAY                  0x000002E0L

//
// %1 Player callsign
//
// %2 SEAD callsign
//
// MessageId: AIC_SEAD_WITH_YOU
//
// MessageText:
//
//  %1, %2, we're with you.%0
//
#define AIC_SEAD_WITH_YOU                0x000002E1L

//
// %1 Player callsign
//
// %2 fighter callsign
//
// MessageId: AIC_SEAD_HEADED_HOME
//
// MessageText:
//
//  %1, %2, we're headed for home%0
//
#define AIC_SEAD_HEADED_HOME             0x000002E2L

//
// %1 SEAD callsign
//
// MessageId: AIC_SEAD_EJECTING
//
// MessageText:
//
//  %1 is ejecting now!%0
//
#define AIC_SEAD_EJECTING                0x000002E3L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_TALLY_FORM
//
// MessageText:
//
//  %1, %2, tally on you, forming up.%0
//
#define AIC_BOMBER_TALLY_FORM            0x000002E4L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_UK_BOMBER_TALLY_FORM
//
// MessageText:
//
//  %1, %2, tally on you lads, forming up.%0
//
#define AIC_UK_BOMBER_TALLY_FORM         0x000002E5L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_FORM_UP
//
// MessageText:
//
//  %1, %2, forming up.%0
//
#define AIC_BOMBER_FORM_UP               0x000002E6L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_UK_BOMBER_FORM_UP
//
// MessageText:
//
//  %1, %2, forming up mates.%0
//
#define AIC_UK_BOMBER_FORM_UP            0x000002E7L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_BREAK_TO_ATTACK
//
// MessageText:
//
//  %1, %2, breaking to attack.%0
//
#define AIC_BOMBER_BREAK_TO_ATTACK       0x000002E8L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_UK_BOMBER_BREAK_TO_ATTACK
//
// MessageText:
//
//  %1, %2, the lads and I are roving in now.%0
//
#define AIC_UK_BOMBER_BREAK_TO_ATTACK    0x000002E9L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_COMMENCE_ATTACK
//
// MessageText:
//
//  %1, %2, commencing attack run.%0
//
#define AIC_BOMBER_COMMENCE_ATTACK       0x000002EAL

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_UK_BOMBER_COMMENCE_ATTACK
//
// MessageText:
//
//  %1, %2, here we go then, commencing attack run.%0
//
#define AIC_UK_BOMBER_COMMENCE_ATTACK    0x000002EBL

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_REFORMING
//
// MessageText:
//
//  %1, %2, reforming.%0
//
#define AIC_BOMBER_REFORMING             0x000002ECL

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_REFORMING_YOU
//
// MessageText:
//
//  %1, %2, reforming on you.%0
//
#define AIC_BOMBER_REFORMING_YOU         0x000002EDL

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_VISUAL_GLAD
//
// MessageText:
//
//  %1, %2, visual on you, glad you're with us.%0
//
#define AIC_BOMBER_VISUAL_GLAD           0x000002EEL

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_UK_BOMBER_VISUAL_GLAD
//
// MessageText:
//
//  %1, %2, Jolly good to have you with us.%0
//
#define AIC_UK_BOMBER_VISUAL_GLAD        0x000002EFL

//
// %1 Clock position
//
// %2 high/low
//
// MessageId: AIC_BOMBER_BANDITS
//
// MessageText:
//
//  Bandits %1 %2.%0
//
#define AIC_BOMBER_BANDITS               0x000002F0L

//
// %1 Clock position
//
// %2 high/low
//
// MessageId: AIC_BOMBER_HEADS_UP_BANDITS
//
// MessageText:
//
//  Heads up, Bandits %1 %2.%0
//
#define AIC_BOMBER_HEADS_UP_BANDITS      0x000002F1L

//
// %1 Clock position
//
// %2 high/low
//
// MessageId: AIC_UK_BOMBER_HEADS_UP_BANDITS
//
// MessageText:
//
//  I say, Bandits %1 %2.%0
//
#define AIC_UK_BOMBER_HEADS_UP_BANDITS   0x000002F2L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_WE_ARE_HIT
//
// MessageText:
//
//  %1, %2, We're Hit!%0
//
#define AIC_BOMBER_WE_ARE_HIT            0x000002F3L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_UK_BOMBER_WE_ARE_HIT
//
// MessageText:
//
//  %1, %2, We have been hit!%0
//
#define AIC_UK_BOMBER_WE_ARE_HIT         0x000002F4L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_DAMAGE
//
// MessageText:
//
//  %1, %2, taking damage!%0
//
#define AIC_BOMBER_DAMAGE                0x000002F5L

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_UK_BOMBER_DAMAGE
//
// MessageText:
//
//  %1, %2, we have taken damage!%0
//
#define AIC_UK_BOMBER_DAMAGE             0x000002F6L

//
// %1 bomber callsign
//
// MessageId: AIC_BOMBER_MAYDAY
//
// MessageText:
//
//  %1 is going in!  Mayday!%0
//
#define AIC_BOMBER_MAYDAY                0x000002F7L

//
// %1 bomber callsign
//
// MessageId: AIC_UK_BOMBER_MAYDAY
//
// MessageText:
//
//  %1 I cannot maintain control! Mayday!%0
//
#define AIC_UK_BOMBER_MAYDAY             0x000002F8L

//
// %1 bomber callsign
//
// MessageId: AIC_BOMBER_EJECT
//
// MessageText:
//
//  %1 ejecting now!%0
//
#define AIC_BOMBER_EJECT                 0x000002F9L

//
// %1 bomber callsign
//
// MessageId: AIC_BOMBER_BOMBS_GONE
//
// MessageText:
//
//  %1, Bombs gone.%0
//
#define AIC_BOMBER_BOMBS_GONE            0x000002FAL

//
// %1 bomber callsign
//
// MessageId: AIC_UK_BOMBER_BOMBS_GONE
//
// MessageText:
//
//  %1, Release%0
//
#define AIC_UK_BOMBER_BOMBS_GONE         0x000002FBL

//
// %1 bomber callsign
//
// MessageId: AIC_BOMBER_BOMBS_AWAY
//
// MessageText:
//
//  %1, Bombs away.%0
//
#define AIC_BOMBER_BOMBS_AWAY            0x000002FCL

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_THANKS
//
// MessageText:
//
//  %1, %2, Thanks for the escort, heading home.%0
//
#define AIC_BOMBER_THANKS                0x000002FDL

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_UK_BOMBER_THANKS
//
// MessageText:
//
//  %1, %2, Thank you for the escort mates, we are heading home.%0
//
#define AIC_UK_BOMBER_THANKS             0x000002FEL

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_BOMBER_RTB
//
// MessageText:
//
//  %1, %2, Returning to base.%0
//
#define AIC_BOMBER_RTB                   0x000002FFL

//
// %1 Player callsign
//
// %2 bomber callsign
//
// MessageId: AIC_UK_BOMBER_RTB
//
// MessageText:
//
//  %1, %2, We are returning to base.%0
//
#define AIC_UK_BOMBER_RTB                0x00000300L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_CONTACT_US_1
//
// MessageText:
//
//  %1, %2, we are under attack by enemy troops, request immediate support.%0
//
#define AIC_GROUND_CONTACT_US_1          0x00000301L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_CONTACT_US_2
//
// MessageText:
//
//  %1, %2, we are under attack!%0
//
#define AIC_GROUND_CONTACT_US_2          0x00000302L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_CONTACT_US_3
//
// MessageText:
//
//  %1, %2, we are under attack, request assistance.%0
//
#define AIC_GROUND_CONTACT_US_3          0x00000303L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_CONTACT_UK_1
//
// MessageText:
//
//  %1, %2, we think we've been discovered, request support.%0
//
#define AIC_GROUND_CONTACT_UK_1          0x00000304L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_CONTACT_UK_2
//
// MessageText:
//
//  %1, %2, we seem to have come under fire.%0
//
#define AIC_GROUND_CONTACT_UK_2          0x00000305L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_CONTACT_UK_3
//
// MessageText:
//
//  %1, %2, we have been discovered, please come quickly.%0
//
#define AIC_GROUND_CONTACT_UK_3          0x00000306L

//
// %1 Player callsign
//
// %2 ground callsign
//
// %3 cardinal direction
//
// MessageId: AIC_GROUND_NEED_HELP_1
//
// MessageText:
//
//  %1, %2, request air strike on enemy units %3 of our position.%0
//
#define AIC_GROUND_NEED_HELP_1           0x00000307L

//
// %1 Player callsign
//
// %2 ground callsign
//
// %3 cardinal direction
//
// MessageId: AIC_GROUND_NEED_HELP_2
//
// MessageText:
//
//  %1, %2, requesting close air support on enemy units %3 of our position.%0
//
#define AIC_GROUND_NEED_HELP_2           0x00000308L

//
// %1 Player callsign
//
// %2 ground callsign
//
// %3 cardinal direction
//
// MessageId: AIC_GROUND_NEED_HELP_3
//
// MessageText:
//
//  %1, %2, we need air support on enemy units %3 of our position.%0
//
#define AIC_GROUND_NEED_HELP_3           0x00000309L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_FRIENDLY_FIRE_US_1
//
// MessageText:
//
//  %1, Check fire!  Check fire!  Your weapons are landing in our perimeter!%0
//
#define AIC_GROUND_FRIENDLY_FIRE_US_1    0x0000030AL

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_FRIENDLY_FIRE_US_2
//
// MessageText:
//
//  %1, Check fire!  There are friendlies in that area!%0
//
#define AIC_GROUND_FRIENDLY_FIRE_US_2    0x0000030BL

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_FRIENDLY_FIRE_US_3
//
// MessageText:
//
//  %1, We're on the ground, watch your fire!%0
//
#define AIC_GROUND_FRIENDLY_FIRE_US_3    0x0000030CL

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_FRIENDLY_FIRE_UK_1
//
// MessageText:
//
//  %1, Please watch your aim.%0
//
#define AIC_GROUND_FRIENDLY_FIRE_UK_1    0x0000030DL

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_FRIENDLY_FIRE_UK_2
//
// MessageText:
//
//  %1, Careful there bloke. . .  you seem to be hitting our men!%0
//
#define AIC_GROUND_FRIENDLY_FIRE_UK_2    0x0000030EL

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_FRIENDLY_FIRE_UK_3
//
// MessageText:
//
//  %1, We are on the same side, no?%0
//
#define AIC_GROUND_FRIENDLY_FIRE_UK_3    0x0000030FL

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_GOOD_US_1
//
// MessageText:
//
//  %1, You got them!  Great job!%0
//
#define AIC_GROUND_GOOD_US_1             0x00000310L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_GOOD_US_2
//
// MessageText:
//
//  %1, Good drop!%0
//
#define AIC_GROUND_GOOD_US_2             0x00000311L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_GOOD_US_3
//
// MessageText:
//
//  %1, Enemy is withdrawing, thanks a lot.%0
//
#define AIC_GROUND_GOOD_US_3             0x00000312L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_GOOD_UK_1
//
// MessageText:
//
//  %1, Jolly good show mates!%0
//
#define AIC_GROUND_GOOD_UK_1             0x00000313L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_GOOD_UK_2
//
// MessageText:
//
//  %1, Good work lads!%0
//
#define AIC_GROUND_GOOD_UK_2             0x00000314L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_GOOD_UK_3
//
// MessageText:
//
//  %1, That's got the buggers on the run!%0
//
#define AIC_GROUND_GOOD_UK_3             0x00000315L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_DEAD_US_1
//
// MessageText:
//
//  %1, %2, We can't hold out any longer. . . %0
//
#define AIC_GROUND_DEAD_US_1             0x00000316L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_DEAD_US_2
//
// MessageText:
//
//  %1, %2, Too late, we're being overrun!%0
//
#define AIC_GROUND_DEAD_US_2             0x00000317L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_DEAD_UK_1
//
// MessageText:
//
//  %1, %2, Sorry mates, we're done for. . . %0
//
#define AIC_GROUND_DEAD_UK_1             0x00000318L

//
// %1 Player callsign
//
// %2 ground callsign
//
// MessageId: AIC_GROUND_DEAD_UK_2
//
// MessageText:
//
//  %1, %2, We've had it lads. . .  God save the queen!%0
//
#define AIC_GROUND_DEAD_UK_2             0x00000319L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_ENEMY_GONE_US
//
// MessageText:
//
//  %1, Be advised enemy forces have withdrawn%0
//
#define AIC_GROUND_ENEMY_GONE_US         0x0000031AL

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_ENEMY_GONE_UK
//
// MessageText:
//
//  %1, The enemy seems to have run away.%0
//
#define AIC_GROUND_ENEMY_GONE_UK         0x0000031BL

//
// MessageId: AIC_GROUND_CALL_1
//
// MessageText:
//
//  this is Mike four seven%0
//
#define AIC_GROUND_CALL_1                0x0000031CL

//
// MessageId: AIC_GROUND_CALL_2
//
// MessageText:
//
//  this is Zulu eight three%0
//
#define AIC_GROUND_CALL_2                0x0000031DL

//
// MessageId: AIC_GROUND_CALL_3
//
// MessageText:
//
//  this is Victor six one%0
//
#define AIC_GROUND_CALL_3                0x0000031EL

//
// MessageId: AIC_GROUND_CALL_4
//
// MessageText:
//
//  this is Sierra four three%0
//
#define AIC_GROUND_CALL_4                0x0000031FL

//
// MessageId: AIC_GROUND_CALL_5
//
// MessageText:
//
//  this is Tango nine five%0
//
#define AIC_GROUND_CALL_5                0x00000320L

//
// MessageId: AIC_GROUND_CALL_6
//
// MessageText:
//
//  this is Juliet eight seven%0
//
#define AIC_GROUND_CALL_6                0x00000321L

//
// MessageId: AIC_GROUND_CALL_7
//
// MessageText:
//
//  this is India five three%0
//
#define AIC_GROUND_CALL_7                0x00000322L

//
// MessageId: AIC_GROUND_CALL_8
//
// MessageText:
//
//  this is Oscar zero zero%0
//
#define AIC_GROUND_CALL_8                0x00000323L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_1
//
// MessageText:
//
//  Any allied aircraft, any allied aircraft this is %1%0
//
#define AIC_SHOT_DOWN_1                  0x00000324L

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_READ_ME
//
// MessageText:
//
//  %1, %2, do you read me?%0
//
#define AIC_SHOT_DOWN_READ_ME            0x00000325L

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_ACK
//
// MessageText:
//
//  %1, %2, acknowledged%0
//
#define AIC_SHOT_DOWN_ACK                0x00000326L

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_HIDING_US
//
// MessageText:
//
//  %1, %2, enemy near! Hiding!%0
//
#define AIC_SHOT_DOWN_HIDING_US          0x00000327L

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_HIDING_UK
//
// MessageText:
//
//  %1, %2, enemy blokes near!%0
//
#define AIC_SHOT_DOWN_HIDING_UK          0x00000328L

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_UNDERSTOOD
//
// MessageText:
//
//  %1, %2, understood%0
//
#define AIC_SHOT_DOWN_UNDERSTOOD         0x00000329L

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_SAFE
//
// MessageText:
//
//  %1, %2, I am down safely!%0
//
#define AIC_SHOT_DOWN_SAFE               0x0000032AL

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_BEAT_UP_US
//
// MessageText:
//
//  %1, %2, I'm pretty beat up, send help!%0
//
#define AIC_SHOT_DOWN_BEAT_UP_US         0x0000032BL

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_BEAT_UP_UK
//
// MessageText:
//
//  %1, %2, I seem to require medical attention, please send help!%0
//
#define AIC_SHOT_DOWN_BEAT_UP_UK         0x0000032CL

//
// MessageId: AIC_WSO_FENCE_IN
//
// MessageText:
//
//  Fence in.%0
//
#define AIC_WSO_FENCE_IN                 0x0000032DL

//
// MessageId: AIC_WSO_FENCE_OUT
//
// MessageText:
//
//  Fence out.%0
//
#define AIC_WSO_FENCE_OUT                0x0000032EL

//
// MessageId: AIC_AWACS_REQUEST_TANKER
//
// MessageText:
//
//  (AWACS) Request Closest Tanker%0
//
#define AIC_AWACS_REQUEST_TANKER         0x0000032FL

//
// %1 Player callsign
//
// %2 AWACS callsign
//
// %3 Tanker callsign
//
// %4 bearing to Tanker (000-359)
//
// %5 range to Tanker
//
// MessageId: AIC_AWACS_CLOSEST_TANKER
//
// MessageText:
//
//  %1, %2, copy, %3 bearing %4, %5%0
//
#define AIC_AWACS_CLOSEST_TANKER         0x00000330L

//
// %1 wingman number
//
// %2 number in middile <one, two, . . .>
//
// %3 bandit type
//
// MessageId: AIC_SORTED_MIDDLE
//
// MessageText:
//
//  %1, sorted middle %2 %3.%0
//
#define AIC_SORTED_MIDDLE                0x00000331L

//
// MessageId: AIC_MISSION_COMPLETED
//
// MessageText:
//
//  MISSION COMPLETED%0
//
#define AIC_MISSION_COMPLETED            0x00000332L

//
// MessageId: AIC_MISSION_FAILED
//
// MessageText:
//
//  MISSION FAILED%0
//
#define AIC_MISSION_FAILED               0x00000333L

//
// MessageId: AIC_WISH_END
//
// MessageText:
//
//  Do You Wish To Exit The Mission?%0
//
#define AIC_WISH_END                     0x00000334L

//
// MessageId: AIC_CONFIRM_EXIT
//
// MessageText:
//
//  Are You Sure You Want To Exit The Mission?%0
//
#define AIC_CONFIRM_EXIT                 0x00000335L

//
// MessageId: AIC_V_FORM
//
// MessageText:
//
//  V Formation%0
//
#define AIC_V_FORM                       0x00000336L

//
// MessageId: AIC_YES
//
// MessageText:
//
//  Yes%0
//
#define AIC_YES                          0x00000337L

//
// MessageId: AIC_NO
//
// MessageText:
//
//  No%0
//
#define AIC_NO                           0x00000338L

//
// %1 <Bandit> or <Enemy Helicopter>
//
// %2 clock position high / low
//
// MessageId: AIC_BANDIT_CALL
//
// MessageText:
//
//  %1 %2%0
//
#define AIC_BANDIT_CALL                  0x00000339L

//  
//
// MessageId: AIC_RADAR_ON
//
// MessageText:
//
//  Radar On%0
//
#define AIC_RADAR_ON                     0x0000033AL

//  
//
// MessageId: AIC_RADAR_OFF
//
// MessageText:
//
//  Radar Off%0
//
#define AIC_RADAR_OFF                    0x0000033BL

//  
//
// MessageId: AIC_MUSIC_ON
//
// MessageText:
//
//  Music On%0
//
#define AIC_MUSIC_ON                     0x0000033CL

//  
//
// MessageId: AIC_MUSIC_OFF
//
// MessageText:
//
//  Music Off%0
//
#define AIC_MUSIC_OFF                    0x0000033DL

//
// MessageId: AIC_JUMPING_TO_ACTION
//
// MessageText:
//
//  Jumping To Next Action Point%0
//
#define AIC_JUMPING_TO_ACTION            0x0000033EL

//
// %1 wind speed
//
// MessageId: AIC_TOWER_WINDS_ARE_CALM
//
// MessageText:
//
//  winds are calm%0
//
#define AIC_TOWER_WINDS_ARE_CALM         0x0000033FL

//
// MessageId: AIC_LOADING_MISSION
//
// MessageText:
//
//  Loading Mission ...%0
//
#define AIC_LOADING_MISSION              0x00000340L

//
// SIMULATION - NEW FOR F/A-18
//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_FINAL_ACLS_CONTACT
//
// MessageText:
//
//  %1 five miles, A C L S lock-on, call your needles%0
//
#define AIC_FINAL_ACLS_CONTACT           0x00000341L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_CONCUR
//
// MessageText:
//
//  %1 concur%0
//
#define AIC_CONCUR                       0x00000342L

//
// %1 Callsign or modex
//
// %2 glideslope
//
// %3 centerline
// Not Translated
//
// MessageId: AIC_FINAL_NO_NEEDLES
//
// MessageText:
//
//  %1 understood %2 %3%0
//
#define AIC_FINAL_NO_NEEDLES             0x00000343L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_FINAL_CALL_BALL
//
// MessageText:
//
//  %1 three quarters of a mile, call the ball%0
//
#define AIC_FINAL_CALL_BALL              0x00000344L

//
// %1 Callsign or modex
//
// %2 bearing
// Not Translated
//
// MessageId: AIC_FINAL_BOLTER1
//
// MessageText:
//
//  %1 climb and maintain one two zero zero feet, when established turn left to downwind %2%0
//
#define AIC_FINAL_BOLTER1                0x00000345L

//
// %1 Callsign or modex
//
// %2 bearing
// Not Translated
//
// MessageId: AIC_FINAL_BOLTER2
//
// MessageText:
//
//  %1 left to bearing %2%0
//
#define AIC_FINAL_BOLTER2                0x00000346L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_SWITCH_MARSHAL
//
// MessageText:
//
//  %1 switch Marshal%0
//
#define AIC_SWITCH_MARSHAL               0x00000347L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_ROGER
//
// MessageText:
//
//  %1 roger%0
//
#define AIC_ROGER                        0x00000348L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_NEGATIVE
//
// MessageText:
//
//  %1 negative%0
//
#define AIC_NEGATIVE                     0x00000349L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_CONTACT_STRIKE
//
// MessageText:
//
//  %1 contact Strike%0
//
#define AIC_CONTACT_STRIKE               0x0000034AL

//
// %1 Callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_COPY_FUEL_STATE
//
// MessageText:
//
//  %1 copy %2%0
//
#define AIC_COPY_FUEL_STATE              0x0000034BL

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_SIGNAL_TANK
//
// MessageText:
//
//  %1 your signal is tank%0
//
#define AIC_SIGNAL_TANK                  0x0000034CL

//
// %1 tanker callsign
//
// %2 altitude 4
// Not Translated
//
// MessageId: AIC_TANKER_OVERHEAD
//
// MessageText:
//
//  %1 is overhead at angels %2%0
//
#define AIC_TANKER_OVERHEAD              0x0000034DL

// Not Translated
//
// MessageId: AIC_DELTA_DELTA
//
// MessageText:
//
//  Delta, Delta%0
//
#define AIC_DELTA_DELTA                  0x0000034EL

//
// %1 minutes
// Not Translated
//
// MessageId: AIC_FOUL_DECK
//
// MessageText:
//
//  We have a foul deck for at least %2 minutes%0
//
#define AIC_FOUL_DECK                    0x0000034FL

//
// %1 Callsign or modex
//
// %2 altitude 4
// Not Translated
//
// MessageId: AIC_DISCO_APPROACH
//
// MessageText:
//
//  %1 discontinue your approach.  Take angels %2 and continues inbound.%0
//
#define AIC_DISCO_APPROACH               0x00000350L

// Not Translated
//
// MessageId: AIC_GREEN_DECK
//
// MessageText:
//
//  Green deck%0
//
#define AIC_GREEN_DECK                   0x00000351L

//
// %1 Callsign or modex
//
// %2 bearing
// Not Translated
//
// MessageId: AIC_NEW_FINAL_BEARING
//
// MessageText:
//
//  %1 new final bearing is %2%0
//
#define AIC_NEW_FINAL_BEARING            0x00000352L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_DIRTY_UP_10
//
// MessageText:
//
//  %1 at ten miles, dirty up%0
//
#define AIC_DIRTY_UP_10                  0x00000353L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_DIRTY_UP_8
//
// MessageText:
//
//  %1 at eight miles, dirty up%0
//
#define AIC_DIRTY_UP_8                   0x00000354L

// Not Translated
//
// MessageId: AIC_STRIKE
//
// MessageText:
//
//  Strike%0
//
#define AIC_STRIKE                       0x00000355L

// Not Translated
//
// MessageId: AIC_MARSHAL
//
// MessageText:
//
//  Marshal%0
//
#define AIC_MARSHAL                      0x00000356L

// Not Translated
//
// MessageId: AIC_FINAL
//
// MessageText:
//
//  Final%0
//
#define AIC_FINAL                        0x00000357L

// Not Translated
//
// MessageId: AIC_DEPARTURE
//
// MessageText:
//
//  Departure%0
//
#define AIC_DEPARTURE                    0x00000358L

// Not Translated
//
// MessageId: AIC_GROUND_CS
//
// MessageText:
//
//  Ground%0
//
#define AIC_GROUND_CS                    0x00000359L

// Not Translated
//
// MessageId: AIC_TOWER_CS
//
// MessageText:
//
//  Tower%0
//
#define AIC_TOWER_CS                     0x0000035AL

// Not Translated
//
// MessageId: AIC_ARRIVAL_CS
//
// MessageText:
//
//  Arrival%0
//
#define AIC_ARRIVAL_CS                   0x0000035BL

// Not Translated
//
// MessageId: AIC_POINT
//
// MessageText:
//
//  point%0
//
#define AIC_POINT                        0x0000035CL

// Not Translated
//
// MessageId: AIC_ONE_MILE
//
// MessageText:
//
//  one mile%0
//
#define AIC_ONE_MILE                     0x0000035DL

// Not Translated
//
// MessageId: AIC_ONE_AND_HALF_MILE
//
// MessageText:
//
//  one and a half miles%0
//
#define AIC_ONE_AND_HALF_MILE            0x0000035EL

// Not Translated
//
// MessageId: AIC_TWO_MILES
//
// MessageText:
//
//  two miles%0
//
#define AIC_TWO_MILES                    0x0000035FL

// Not Translated
//
// MessageId: AIC_TWO_AND_HALF_MILES
//
// MessageText:
//
//  two and a half miles%0
//
#define AIC_TWO_AND_HALF_MILES           0x00000360L

// Not Translated
//
// MessageId: AIC_THREE_MILES
//
// MessageText:
//
//  three miles%0
//
#define AIC_THREE_MILES                  0x00000361L

// Not Translated
//
// MessageId: AIC_THREE_AND_HALF_MILES
//
// MessageText:
//
//  three and a half miles%0
//
#define AIC_THREE_AND_HALF_MILES         0x00000362L

// Not Translated
//
// MessageId: AIC_FOUR_MILES
//
// MessageText:
//
//  four miles%0
//
#define AIC_FOUR_MILES                   0x00000363L

// Not Translated
//
// MessageId: AIC_FOUR_AND_HALF_MILES
//
// MessageText:
//
//  four and a half miles%0
//
#define AIC_FOUR_AND_HALF_MILES          0x00000364L

// Not Translated
//
// MessageId: AIC_ON_GLIDESLOPE
//
// MessageText:
//
//  on glideslope%0
//
#define AIC_ON_GLIDESLOPE                0x00000365L

// Not Translated
//
// MessageId: AIC_SLIGHTLY_BELOW_GS
//
// MessageText:
//
//  slightly below glideslope%0
//
#define AIC_SLIGHTLY_BELOW_GS            0x00000366L

// Not Translated
//
// MessageId: AIC_BELOW_GS
//
// MessageText:
//
//  below glideslope%0
//
#define AIC_BELOW_GS                     0x00000367L

// Not Translated
//
// MessageId: AIC_SLIGHTLY_ABOVE_GS
//
// MessageText:
//
//  slightly above glideslope%0
//
#define AIC_SLIGHTLY_ABOVE_GS            0x00000368L

// Not Translated
//
// MessageId: AIC_ABOVE_GS
//
// MessageText:
//
//  above glideslope%0
//
#define AIC_ABOVE_GS                     0x00000369L

// Not Translated
//
// MessageId: AIC_ON_CENTERLINE
//
// MessageText:
//
//  on centerline%0
//
#define AIC_ON_CENTERLINE                0x0000036AL

// Not Translated
//
// MessageId: AIC_SLIGHTLY_LEFT_CL
//
// MessageText:
//
//  slightly left of centerline%0
//
#define AIC_SLIGHTLY_LEFT_CL             0x0000036BL

// Not Translated
//
// MessageId: AIC_LEFT_CL
//
// MessageText:
//
//  left of centerline%0
//
#define AIC_LEFT_CL                      0x0000036CL

// Not Translated
//
// MessageId: AIC_SLIGHTLY_RIGHT_CL
//
// MessageText:
//
//  slightly right of centerline%0
//
#define AIC_SLIGHTLY_RIGHT_CL            0x0000036DL

// Not Translated
//
// MessageId: AIC_RIGHT_CL
//
// MessageText:
//
//  right of centerline%0
//
#define AIC_RIGHT_CL                     0x0000036EL

//
// %1 Callsign or modex
//
// %2 ceiling
//
// %3 visability
// Not Translated
//
// MessageId: AIC_MARSHAL_CHECK
//
// MessageText:
//
//  %1, Marshal.  Mother's weather, ceiling %2, %3%0
//
#define AIC_MARSHAL_CHECK                0x0000036FL

//
// %1 bearing
//
// %2 range
//
// %3 altitude 4
// Not Translated
//
// MessageId: AIC_CASE_III
//
// MessageText:
//
//  Case three recoveries.  Marshal on the %1 radial, %2, angels %3.  %0
//
#define AIC_CASE_III                     0x00000370L

//
// %1 expected approach time
//
// %2 time
// Not Translated
//
// MessageId: AIC_EXPECTED_APPROACH
//
// MessageText:
//
//  Expected approach time %1.  Time now %2.%0
//
#define AIC_EXPECTED_APPROACH            0x00000371L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_COMMENCE_NOW
//
// MessageText:
//
//  %1 commence now%0
//
#define AIC_COMMENCE_NOW                 0x00000372L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_WHAT_DOING
//
// MessageText:
//
//  %1 what are you doing?%0
//
#define AIC_WHAT_DOING                   0x00000373L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_REMAIN_MARSHAL_1
//
// MessageText:
//
//  %1 remain in the marshal stack%0
//
#define AIC_REMAIN_MARSHAL_1             0x00000374L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_REMAIN_MARSHAL_2
//
// MessageText:
//
//  %1 return to the marshal stack%0
//
#define AIC_REMAIN_MARSHAL_2             0x00000375L

//
// %1 Callsign or modex
//
// %2 bearing
// Not Translated
//
// MessageId: AIC_MANUAL_PUSH_1
//
// MessageText:
//
//  %1 Marshal, we have a ready deck.  These will be vectors to a manual push.  Take heading %2%0
//
#define AIC_MANUAL_PUSH_1                0x00000376L

// Not Translated
//
// MessageId: AIC_MANUAL_PUSH_DESCEND
//
// MessageText:
//
//  descend and maintain angels one point two.%0
//
#define AIC_MANUAL_PUSH_DESCEND          0x00000377L

// Not Translated
//
// MessageId: AIC_MANUAL_PUSH_CLIMB
//
// MessageText:
//
//  climb and maintain angels one point two.%0
//
#define AIC_MANUAL_PUSH_CLIMB            0x00000378L

// Not Translated
//
// MessageId: AIC_MANUAL_PUSH_MAINTAIN
//
// MessageText:
//
//  maintain angels one point two.%0
//
#define AIC_MANUAL_PUSH_MAINTAIN         0x00000379L

//
// %1 ten thousands digit
//
// %2 thousands digit
// Not Translated
//
// MessageId: AIC_CLOUD_DECK
//
// MessageText:
//
//  %1%2 kay%0
//
#define AIC_CLOUD_DECK                   0x0000037AL

// Not Translated
//
// MessageId: AIC_UNLIMITED
//
// MessageText:
//
//  unlimited%0
//
#define AIC_UNLIMITED                    0x0000037BL

//
// %1 cloud deck
// Not Translated
//
// MessageId: AIC_SCATTERED_CD
//
// MessageText:
//
//  scattered %1%0
//
#define AIC_SCATTERED_CD                 0x0000037CL

//
// %1 cloud deck
// Not Translated
//
// MessageId: AIC_BROKEN_CD
//
// MessageText:
//
//  broken %1%0
//
#define AIC_BROKEN_CD                    0x0000037DL

//
// %1 cloud deck
// Not Translated
//
// MessageId: AIC_OVERCAST_CD
//
// MessageText:
//
//  overcast %1%0
//
#define AIC_OVERCAST_CD                  0x0000037EL

//
// %1 range
// Not Translated
//
// MessageId: AIC_VISIBILITY
//
// MessageText:
//
//  visability %1%0
//
#define AIC_VISIBILITY                   0x0000037FL

// Not Translated
//
// MessageId: AIC_LITTLE_HIGH
//
// MessageText:
//
//  You're a little high%0
//
#define AIC_LITTLE_HIGH                  0x00000380L

// Not Translated
//
// MessageId: AIC_U_HIGH
//
// MessageText:
//
//  You're high%0
//
#define AIC_U_HIGH                       0x00000381L

// Not Translated
//
// MessageId: AIC_LITTLE_LOW
//
// MessageText:
//
//  You're a little low%0
//
#define AIC_LITTLE_LOW                   0x00000382L

// Not Translated
//
// MessageId: AIC_U_LOW
//
// MessageText:
//
//  You're low%0
//
#define AIC_U_LOW                        0x00000383L

// Not Translated
//
// MessageId: AIC_GOING_HIGH
//
// MessageText:
//
//  You're going high%0
//
#define AIC_GOING_HIGH                   0x00000384L

// Not Translated
//
// MessageId: AIC_GOING_LOW
//
// MessageText:
//
//  You're going low%0
//
#define AIC_GOING_LOW                    0x00000385L

// Not Translated
//
// MessageId: AIC_U_ON_CENTERLINE
//
// MessageText:
//
//  You're on centerline%0
//
#define AIC_U_ON_CENTERLINE              0x00000386L

// Not Translated
//
// MessageId: AIC_U_ON_GLIDESLOPE
//
// MessageText:
//
//  You're on glideslope%0
//
#define AIC_U_ON_GLIDESLOPE              0x00000387L

// Not Translated
//
// MessageId: AIC_U_LINED_LEFT
//
// MessageText:
//
//  You're lined up left%0
//
#define AIC_U_LINED_LEFT                 0x00000388L

// Not Translated
//
// MessageId: AIC_U_LINED_RIGHT
//
// MessageText:
//
//  You're lined up right%0
//
#define AIC_U_LINED_RIGHT                0x00000389L

// Not Translated
//
// MessageId: AIC_U_DRIFTING_LEFT
//
// MessageText:
//
//  You're drifting left%0
//
#define AIC_U_DRIFTING_LEFT              0x0000038AL

// Not Translated
//
// MessageId: AIC_U_DRIFTING_RIGHT
//
// MessageText:
//
//  You're drifting right%0
//
#define AIC_U_DRIFTING_RIGHT             0x0000038BL

// Not Translated
//
// MessageId: AIC_U_FAST
//
// MessageText:
//
//  You're fast%0
//
#define AIC_U_FAST                       0x0000038CL

// Not Translated
//
// MessageId: AIC_U_SLOW
//
// MessageText:
//
//  You're slow%0
//
#define AIC_U_SLOW                       0x0000038DL

// Not Translated
//
// MessageId: AIC_ROGER_BALL
//
// MessageText:
//
//  Roger ball%0
//
#define AIC_ROGER_BALL                   0x0000038EL

// Not Translated
//
// MessageId: AIC_KEEP_COMING
//
// MessageText:
//
//  Keep coming%0
//
#define AIC_KEEP_COMING                  0x0000038FL

// Not Translated
//
// MessageId: AIC_PADDLES_CONTACT
//
// MessageText:
//
//  Paddles contact%0
//
#define AIC_PADDLES_CONTACT              0x00000390L

// Not Translated
//
// MessageId: AIC_DECK_LITTLE_UP
//
// MessageText:
//
//  The deck is moving up a little%0
//
#define AIC_DECK_LITTLE_UP               0x00000391L

// Not Translated
//
// MessageId: AIC_DECK_UP
//
// MessageText:
//
//  The deck is moving up%0
//
#define AIC_DECK_UP                      0x00000392L

// Not Translated
//
// MessageId: AIC_DECK_LITTLE_DOWN
//
// MessageText:
//
//  The deck is moving down a little%0
//
#define AIC_DECK_LITTLE_DOWN             0x00000393L

// Not Translated
//
// MessageId: AIC_DECK_DOWN
//
// MessageText:
//
//  The deck is moving down%0
//
#define AIC_DECK_DOWN                    0x00000394L

// Not Translated
//
// MessageId: AIC_DECK_STEADY
//
// MessageText:
//
//  The deck is steady%0
//
#define AIC_DECK_STEADY                  0x00000395L

// Not Translated
//
// MessageId: AIC_WIND_S_STARBOARD
//
// MessageText:
//
//  Winds are slightly starboard%0
//
#define AIC_WIND_S_STARBOARD             0x00000396L

// Not Translated
//
// MessageId: AIC_WIND_STARBOARD
//
// MessageText:
//
//  Winds are starboard%0
//
#define AIC_WIND_STARBOARD               0x00000397L

// Not Translated
//
// MessageId: AIC_WIND_S_PORT
//
// MessageText:
//
//  Winds are slightly port%0
//
#define AIC_WIND_S_PORT                  0x00000398L

// Not Translated
//
// MessageId: AIC_WIND_PORT
//
// MessageText:
//
//  Winds are port%0
//
#define AIC_WIND_PORT                    0x00000399L

// Not Translated
//
// MessageId: AIC_WIND_AXIAL
//
// MessageText:
//
//  Winds are axial%0
//
#define AIC_WIND_AXIAL                   0x0000039AL

// Not Translated
//
// MessageId: AIC_U_UNDERPOWERED
//
// MessageText:
//
//  You're underpowered%0
//
#define AIC_U_UNDERPOWERED               0x0000039BL

// Not Translated
//
// MessageId: AIC_U_OVERPOWERED
//
// MessageText:
//
//  You're overpowered%0
//
#define AIC_U_OVERPOWERED                0x0000039CL

// Not Translated
//
// MessageId: AIC_SHIP_STARBOARD_TURN
//
// MessageText:
//
//  Ship's in a starboard turn%0
//
#define AIC_SHIP_STARBOARD_TURN          0x0000039DL

// Not Translated
//
// MessageId: AIC_SHIP_PORT_TURN
//
// MessageText:
//
//  Ship's in a port turn%0
//
#define AIC_SHIP_PORT_TURN               0x0000039EL

// Not Translated
//
// MessageId: AIC_KEEP_TURN_IN
//
// MessageText:
//
//  Keep your turn in.%0
//
#define AIC_KEEP_TURN_IN                 0x0000039FL

// Not Translated
//
// MessageId: AIC_CHECK_LINEUP
//
// MessageText:
//
//  Check your lineup%0
//
#define AIC_CHECK_LINEUP                 0x000003A0L

// Not Translated
//
// MessageId: AIC_BACK_RIGHT
//
// MessageText:
//
//  Back to the right%0
//
#define AIC_BACK_RIGHT                   0x000003A1L

// Not Translated
//
// MessageId: AIC_BACK_LEFT
//
// MessageText:
//
//  Back to the left%0
//
#define AIC_BACK_LEFT                    0x000003A2L

// Not Translated
//
// MessageId: AIC_DO_NOT_SETTLE
//
// MessageText:
//
//  Don't settle%0
//
#define AIC_DO_NOT_SETTLE                0x000003A3L

// Not Translated
//
// MessageId: AIC_DO_NOT_GO_LOW
//
// MessageText:
//
//  Don't go low%0
//
#define AIC_DO_NOT_GO_LOW                0x000003A4L

// Not Translated
//
// MessageId: AIC_DO_NOT_CLIMB
//
// MessageText:
//
//  Don't climb%0
//
#define AIC_DO_NOT_CLIMB                 0x000003A5L

// Not Translated
//
// MessageId: AIC_DO_NOT_GO_HIGH
//
// MessageText:
//
//  Don't go high%0
//
#define AIC_DO_NOT_GO_HIGH               0x000003A6L

// Not Translated
//
// MessageId: AIC_NO_LOWER
//
// MessageText:
//
//  Don't go any lower%0
//
#define AIC_NO_LOWER                     0x000003A7L

// Not Translated
//
// MessageId: AIC_NO_HIGHER
//
// MessageText:
//
//  Don't go any higher%0
//
#define AIC_NO_HIGHER                    0x000003A8L

// Not Translated
//
// MessageId: AIC_POWER_ON
//
// MessageText:
//
//  Power back on%0
//
#define AIC_POWER_ON                     0x000003A9L

// Not Translated
//
// MessageId: AIC_NO_SETTLE_THROUGH
//
// MessageText:
//
//  Don't settle through it%0
//
#define AIC_NO_SETTLE_THROUGH            0x000003AAL

// Not Translated
//
// MessageId: AIC_HOLD_U_GOT
//
// MessageText:
//
//  Hold what you've got%0
//
#define AIC_HOLD_U_GOT                   0x000003ABL

// Not Translated
//
// MessageId: AIC_FLY_BALL
//
// MessageText:
//
//  Fly the ball%0
//
#define AIC_FLY_BALL                     0x000003ACL

// Not Translated
//
// MessageId: AIC_EASY
//
// MessageText:
//
//  Easy with it%0
//
#define AIC_EASY                         0x000003ADL

// Not Translated
//
// MessageId: AIC_EASY_WINGS
//
// MessageText:
//
//  Easy with your wings%0
//
#define AIC_EASY_WINGS                   0x000003AEL

// Not Translated
//
// MessageId: AIC_LITTLE_POWER
//
// MessageText:
//
//  A little power%0
//
#define AIC_LITTLE_POWER                 0x000003AFL

// Not Translated
//
// MessageId: AIC_POWER
//
// MessageText:
//
//  Power%0
//
#define AIC_POWER                        0x000003B0L

// Not Translated
//
// MessageId: AIC_ATTITUDE
//
// MessageText:
//
//  Attitude%0
//
#define AIC_ATTITUDE                     0x000003B1L

// Not Translated
//
// MessageId: AIC_LITTLE_ATTITUDE
//
// MessageText:
//
//  A little attitude%0
//
#define AIC_LITTLE_ATTITUDE              0x000003B2L

// Not Translated
//
// MessageId: AIC_LITTLE_R_RUDDER
//
// MessageText:
//
//  A little right rudder%0
//
#define AIC_LITTLE_R_RUDDER              0x000003B3L

// Not Translated
//
// MessageId: AIC_LITTLE_L_RUDDER
//
// MessageText:
//
//  A little left rudder%0
//
#define AIC_LITTLE_L_RUDDER              0x000003B4L

// Not Translated
//
// MessageId: AIC_LEFT_RUDDER
//
// MessageText:
//
//  Left rudder%0
//
#define AIC_LEFT_RUDDER                  0x000003B5L

// Not Translated
//
// MessageId: AIC_RIGHT_RUDDER
//
// MessageText:
//
//  Right rudder%0
//
#define AIC_RIGHT_RUDDER                 0x000003B6L

// Not Translated
//
// MessageId: AIC_LITTLE_R_LINEUP
//
// MessageText:
//
//  A little right for lineup%0
//
#define AIC_LITTLE_R_LINEUP              0x000003B7L

// Not Translated
//
// MessageId: AIC_COME_LITTLE_LEFT
//
// MessageText:
//
//  Come a little left%0
//
#define AIC_COME_LITTLE_LEFT             0x000003B8L

// Not Translated
//
// MessageId: AIC_RIGHT_LINEUP
//
// MessageText:
//
//  Right for lineup%0
//
#define AIC_RIGHT_LINEUP                 0x000003B9L

// Not Translated
//
// MessageId: AIC_COME_LEFT
//
// MessageText:
//
//  Come left%0
//
#define AIC_COME_LEFT                    0x000003BAL

// Not Translated
//
// MessageId: AIC_WAVEOFF
//
// MessageText:
//
//  Waveoff%0
//
#define AIC_WAVEOFF                      0x000003BBL

// Not Translated
//
// MessageId: AIC_WAVEOFF_DECK
//
// MessageText:
//
//  Waveoff, foul deck%0
//
#define AIC_WAVEOFF_DECK                 0x000003BCL

// Not Translated
//
// MessageId: AIC_CUT
//
// MessageText:
//
//  Cut%0
//
#define AIC_CUT                          0x000003BDL

// Not Translated
//
// MessageId: AIC_DROP_HOOK
//
// MessageText:
//
//  Drop your hook%0
//
#define AIC_DROP_HOOK                    0x000003BEL

// Not Translated
//
// MessageId: AIC_DROP_GEAR
//
// MessageText:
//
//  Drop your gear%0
//
#define AIC_DROP_GEAR                    0x000003BFL

// Not Translated
//
// MessageId: AIC_DROP_FLAPS
//
// MessageText:
//
//  Drop your flaps%0
//
#define AIC_DROP_FLAPS                   0x000003C0L

// Not Translated
//
// MessageId: AIC_LEVEL_WINGS
//
// MessageText:
//
//  Level your wings%0
//
#define AIC_LEVEL_WINGS                  0x000003C1L

// Not Translated
//
// MessageId: AIC_POWER_INTENSE
//
// MessageText:
//
//  Power!%0
//
#define AIC_POWER_INTENSE                0x000003C2L

// Not Translated
//
// MessageId: AIC_BURNER
//
// MessageText:
//
//  Burner!%0
//
#define AIC_BURNER                       0x000003C3L

// Not Translated
//
// MessageId: AIC_ATTITUDE_INTENSE
//
// MessageText:
//
//  Attitude!%0
//
#define AIC_ATTITUDE_INTENSE             0x000003C4L

// Not Translated
//
// MessageId: AIC_LEFT_RUDDER_INTENSE
//
// MessageText:
//
//  Left rudder!%0
//
#define AIC_LEFT_RUDDER_INTENSE          0x000003C5L

// Not Translated
//
// MessageId: AIC_RIGHT_RUDDER_INTENSE
//
// MessageText:
//
//  Right rudder!%0
//
#define AIC_RIGHT_RUDDER_INTENSE         0x000003C6L

// Not Translated
//
// MessageId: AIC_RIGHT_LINEUP_INTENSE
//
// MessageText:
//
//  Right for lineup!%0
//
#define AIC_RIGHT_LINEUP_INTENSE         0x000003C7L

// Not Translated
//
// MessageId: AIC_COME_LEFT_INTENSE
//
// MessageText:
//
//  Come left!%0
//
#define AIC_COME_LEFT_INTENSE            0x000003C8L

// Not Translated
//
// MessageId: AIC_BOLTER
//
// MessageText:
//
//  Bolter!%0
//
#define AIC_BOLTER                       0x000003C9L

// Not Translated
//
// MessageId: AIC_WAVEOFF_INTENSE
//
// MessageText:
//
//  Waveoff!%0
//
#define AIC_WAVEOFF_INTENSE              0x000003CAL

// Not Translated
//
// MessageId: AIC_WAVEOFF_3
//
// MessageText:
//
//  Waveoff, Waveoff, Waveoff!%0
//
#define AIC_WAVEOFF_3                    0x000003CBL

// Not Translated
//
// MessageId: AIC_DROP_HOOK_INTENSE
//
// MessageText:
//
//  Drop your hook!%0
//
#define AIC_DROP_HOOK_INTENSE            0x000003CCL

// Not Translated
//
// MessageId: AIC_DROP_GEAR_INTENSE
//
// MessageText:
//
//  Drop your gear!%0
//
#define AIC_DROP_GEAR_INTENSE            0x000003CDL

// Not Translated
//
// MessageId: AIC_DROP_FLAPS_INTENSE
//
// MessageText:
//
//  Drop your flaps!%0
//
#define AIC_DROP_FLAPS_INTENSE           0x000003CEL

// Not Translated
//
// MessageId: AIC_LEVEL_WINGS_INTENSE
//
// MessageText:
//
//  Level your wings!%0
//
#define AIC_LEVEL_WINGS_INTENSE          0x000003CFL

// Not Translated
//
// MessageId: AIC_CLIMB_INTENSE
//
// MessageText:
//
//  Climb!%0
//
#define AIC_CLIMB_INTENSE                0x000003D0L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_SWEET_OUT
//
// MessageText:
//
//  %1 sweet and sweet.  Continue outbound.%0
//
#define AIC_STRIKE_SWEET_OUT             0x000003D1L

//
// %1 Callsign or modex
//
// %2 AWACS callsign
// Not Translated
//
// MessageId: AIC_STRIKE_SWITCH_CONTACT
//
// MessageText:
//
//  %1 switching control, contact %2%0
//
#define AIC_STRIKE_SWITCH_CONTACT        0x000003D2L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_CLEARED_IN
//
// MessageText:
//
//  %1 Strike, cleared inbound%0
//
#define AIC_STRIKE_CLEARED_IN            0x000003D3L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_SWITCH_FINAL
//
// MessageText:
//
//  %1 switch Final%0
//
#define AIC_STRIKE_SWITCH_FINAL          0x000003D4L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_STRIKE_AWACS_MIDNIGHT
//
// MessageText:
//
//  %1 is down, Midnight, repeat Midnight.  Standby for local control.%0
//
#define AIC_STRIKE_AWACS_MIDNIGHT        0x000003D5L

// Not Translated
//
// MessageId: AIC_STRIKE_SUNRISE_2
//
// MessageText:
//
//  Strike, assuming control.  Sunrise, repeat Sunrise.%0
//
#define AIC_STRIKE_SUNRISE_2             0x000003D6L

// Not Translated
//
// MessageId: AIC_STRIKE_SUNRISE
//
// MessageText:
//
//  Strike, sunrise.%0
//
#define AIC_STRIKE_SUNRISE               0x000003D7L

// Not Translated
//
// MessageId: AIC_STRIKE_MIDNIGHT
//
// MessageText:
//
//  Strike, midnight%0
//
#define AIC_STRIKE_MIDNIGHT              0x000003D8L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_CLEAN
//
// MessageText:
//
//  %1 clean%0
//
#define AIC_STRIKE_CLEAN                 0x000003D9L

//
// %1 Callsign or modex
//
// %2 bearing
//
// %3 range
//
// %4 altitude
//
// %5 aspect
//
// %6 speed
// Not Translated
//
// MessageId: AIC_STRIKE_BEARING
//
// MessageText:
//
//  %1 bearing %2, %3, %4, %5, %6%0
//
#define AIC_STRIKE_BEARING               0x000003DAL

//
// %1 Callsign or modex
//
// %2 bearing
//
// %3 range
//
// %4 altitude
//
// %5 aspect
//
// %6 speed
// Not Translated
//
// MessageId: AIC_STRIKE_THREAT_BEARING
//
// MessageText:
//
//  %1 threat bearing %2, %3, %4, %5, %6%0
//
#define AIC_STRIKE_THREAT_BEARING        0x000003DBL

//
// %1 Callsign or modex
//
// %2 bearing
//
// %3 range
//
// %4 altitude
//
// %5 aspect
//
// %6 speed
// Not Translated
//
// MessageId: AIC_STRIKE_FRIENDLY_BEARING
//
// MessageText:
//
//  %1 friendly traffic bearing %2, %3, %4, %5, %6%0
//
#define AIC_STRIKE_FRIENDLY_BEARING      0x000003DCL

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_MERGED
//
// MessageText:
//
//  %1 Strike, merged%0
//
#define AIC_STRIKE_MERGED                0x000003DDL

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_COPY
//
// MessageText:
//
//  %1 Strike, copy%0
//
#define AIC_STRIKE_COPY                  0x000003DEL

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_ACKNOWLEDGED
//
// MessageText:
//
//  %1 Strike, acknowledged%0
//
#define AIC_STRIKE_ACKNOWLEDGED          0x000003DFL

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_SAR_ENROUTE
//
// MessageText:
//
//  %1 Strike, copy, SAR package enroute%0
//
#define AIC_STRIKE_SAR_ENROUTE           0x000003E0L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_NEGATIVE
//
// MessageText:
//
//  %1 Strike, negative%0
//
#define AIC_STRIKE_NEGATIVE              0x000003E1L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_VECTORING_FIGHTERS
//
// MessageText:
//
//  %1 Strike, copy, vectoring fighters to you location.%0
//
#define AIC_STRIKE_VECTORING_FIGHTERS    0x000003E2L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_HELP_ON_WAY
//
// MessageText:
//
//  %1 Strike, standby, help is on the way.%0
//
#define AIC_STRIKE_HELP_ON_WAY           0x000003E3L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_NO_ASSETS
//
// MessageText:
//
//  %1 Strike, negative, no assets available.%0
//
#define AIC_STRIKE_NO_ASSETS             0x000003E4L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_WEASELS_INBOUND
//
// MessageText:
//
//  %1 Strike, copy Weasel support inbound%0
//
#define AIC_STRIKE_WEASELS_INBOUND       0x000003E5L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_NO_WEASELS
//
// MessageText:
//
//  %1 Strike, negative, no Weasels available%0
//
#define AIC_STRIKE_NO_WEASELS            0x000003E6L

//
// %1 cardinal
// Not Translated
//
// MessageId: AIC_STRIKE_SCRAMMING
//
// MessageText:
//
//  Strike, Mother is scramming%0
//
#define AIC_STRIKE_SCRAMMING             0x000003E7L

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_STRIKE_VAMPIRE
//
// MessageText:
//
//  Vampires inbound bearing %1%0
//
#define AIC_STRIKE_VAMPIRE               0x000003E8L

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_STRIKE_ADDITIONAL_VAMPIRES
//
// MessageText:
//
//  Additional Vampires now inbound bearing %1%0
//
#define AIC_STRIKE_ADDITIONAL_VAMPIRES   0x000003E9L

// Not Translated
//
// MessageId: AIC_STIKE_INBOUND_HOSTILES
//
// MessageText:
//
//  Inbound hostiles, Birds Affirm%0
//
#define AIC_STIKE_INBOUND_HOSTILES       0x000003EAL

// Not Translated
//
// MessageId: AIC_STRIKE_INBOUND_VAMPIRES
//
// MessageText:
//
//  Inbound vampires, Birds Affirm%0
//
#define AIC_STRIKE_INBOUND_VAMPIRES      0x000003EBL

// Not Translated
//
// MessageId: AIC_STRIKE_BIRDS_AWAY
//
// MessageText:
//
//  Birds away!%0
//
#define AIC_STRIKE_BIRDS_AWAY            0x000003ECL

// Not Translated
//
// MessageId: AIC_STRIKE_HIT_ALPHA
//
// MessageText:
//
//  Hit Alpha!%0
//
#define AIC_STRIKE_HIT_ALPHA             0x000003EDL

// Not Translated
//
// MessageId: AIC_STRIKE_HIT_ALPHA_VAMPS
//
// MessageText:
//
//  Hit Alpha!  Vampire impact on mother!%0
//
#define AIC_STRIKE_HIT_ALPHA_VAMPS       0x000003EEL

// Not Translated
//
// MessageId: AIC_STRIKE_MOTHER_ABANDONED
//
// MessageText:
//
//  All chicks, Mother is being abandoned at this time.%0
//
#define AIC_STRIKE_MOTHER_ABANDONED      0x000003EFL

// Not Translated
//
// MessageId: AIC_STRIKE_MOTHER_SICK
//
// MessageText:
//
//  Mother sick, flight deck closed.%0
//
#define AIC_STRIKE_MOTHER_SICK           0x000003F0L

// Not Translated
//
// MessageId: AIC_STRIKE_GREEN_DECK
//
// MessageText:
//
//  Green deck, flight deck now open%0
//
#define AIC_STRIKE_GREEN_DECK            0x000003F1L

// Not Translated
//
// MessageId: AIC_STRIKE_MOTHER_CLOSED
//
// MessageText:
//
//  All chicks, Strike, Your signal is divert, mother is closed.%0
//
#define AIC_STRIKE_MOTHER_CLOSED         0x000003F2L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_DIVERT
//
// MessageText:
//
//  %1 Strike, your signal is divert.  Acknowledge.%0
//
#define AIC_STRIKE_DIVERT                0x000003F3L

// Not Translated
//
// MessageId: AIC_STRIKE_CONTACT_FADED
//
// MessageText:
//
//  Strike, contact faded.%0
//
#define AIC_STRIKE_CONTACT_FADED         0x000003F4L

// Not Translated
//
// MessageId: AIC_STRIKE_NEW_PICTURE
//
// MessageText:
//
//  Strike, new picture.%0
//
#define AIC_STRIKE_NEW_PICTURE           0x000003F5L

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_STRIKE_STROBE_BEARING
//
// MessageText:
//
//  Strike, strobe bearing %1%0
//
#define AIC_STRIKE_STROBE_BEARING        0x000003F6L

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_STRIKE_MUSIC_BEARING
//
// MessageText:
//
//  Strike, music %1%0
//
#define AIC_STRIKE_MUSIC_BEARING         0x000003F7L

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_STRIKE_BURNTHROUGH
//
// MessageText:
//
//  Strike, burnthrough %1%0
//
#define AIC_STRIKE_BURNTHROUGH           0x000003F8L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_BROADCAST
//
// MessageText:
//
//  %1 copy broadcast%0
//
#define AIC_STRIKE_BROADCAST             0x000003F9L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_TACTICAL
//
// MessageText:
//
//  %1 copy tactical%0
//
#define AIC_STRIKE_TACTICAL              0x000003FAL

//
// %1 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_STRIKE_LONE_GROUP
//
// MessageText:
//
//  Strike, lone group, %1%0
//
#define AIC_STRIKE_LONE_GROUP            0x000003FBL

//
// %1 number of groups
// Not Translated
//
// MessageId: AIC_STRIKE_GROUPS
//
// MessageText:
//
//  Strike %1 groups%0
//
#define AIC_STRIKE_GROUPS                0x000003FCL

//
// %1 order
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_STRIKE_GROUP
//
// MessageText:
//
//  %1 group %2.%0
//
#define AIC_STRIKE_GROUP                 0x000003FDL

//
// %1 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_STRIKE_POPUP_GROUP
//
// MessageText:
//
//  Strike, popup group, %1%0
//
#define AIC_STRIKE_POPUP_GROUP           0x000003FEL

//
// %1 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_STRIKE_LONE_SINGLE
//
// MessageText:
//
//  Strike, lone single, %1, %2 %3.%0
//
#define AIC_STRIKE_LONE_SINGLE           0x000003FFL

//
// %1 number of groups
// Not Translated
//
// MessageId: AIC_STRIKE_SINGLES
//
// MessageText:
//
//  Strike %1 singles%0
//
#define AIC_STRIKE_SINGLES               0x00000400L

//
// %1 order
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_STRIKE_SINGLE
//
// MessageText:
//
//  %1 single %2%0
//
#define AIC_STRIKE_SINGLE                0x00000401L

//
// %1 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_STRIKE_POPUP_SINGLE
//
// MessageText:
//
//  Strike, popup single, %1%0
//
#define AIC_STRIKE_POPUP_SINGLE          0x00000402L

//
// %1 Flight callsign
//
// %2 contact name
//
// %3 group
// Not Translated
//
// MessageId: AIC_STRIKE_TARGET
//
// MessageText:
//
//  %1 target %2 %3%0
//
#define AIC_STRIKE_TARGET                0x00000403L

//
// %1 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_STRIKE_SPITTER
//
// MessageText:
//
//  Strike spitter %1%0
//
#define AIC_STRIKE_SPITTER               0x00000404L

//
// %1 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_STRIKE_LEAKERS
//
// MessageText:
//
//  Strike leakers %1%0
//
#define AIC_STRIKE_LEAKERS               0x00000405L

// Not Translated
//
// MessageId: AIC_STRIKE_BITTERSWEET
//
// MessageText:
//
//  Strike, bittersweet%0
//
#define AIC_STRIKE_BITTERSWEET           0x00000406L

//
// %1 Player Callsign
// Not Translated
//
// MessageId: AIC_STRIKE_ADD_TARGETS_DOLLY
//
// MessageText:
//
//  %1 transmitting additional targets on dolly%0
//
#define AIC_STRIKE_ADD_TARGETS_DOLLY     0x00000407L

//
// %1 Player Callsign
// Not Translated
//
// MessageId: AIC_STRIKE_NEW_TARGETS_DOLLY
//
// MessageText:
//
//  %1 transmitting new targets on dolly%0
//
#define AIC_STRIKE_NEW_TARGETS_DOLLY     0x00000408L

// Not Translated
//
// MessageId: AIC_STRIKE_ROE_RED
//
// MessageText:
//
//  Strike, Warning Red repeat warning red%0
//
#define AIC_STRIKE_ROE_RED               0x00000409L

// Not Translated
//
// MessageId: AIC_STIKE_ROE_YELLOW
//
// MessageText:
//
//  Strike, Warning Yellow repeat yellow%0
//
#define AIC_STIKE_ROE_YELLOW             0x0000040AL

// Not Translated
//
// MessageId: AIC_STRIKE_ROE_WHITE
//
// MessageText:
//
//  Strike, Warning White repeat white%0
//
#define AIC_STRIKE_ROE_WHITE             0x0000040BL

// Not Translated
//
// MessageId: AIC_STRIKE_WEAPONS_FREE
//
// MessageText:
//
//  Strike, Weapons Free repeat weapons free%0
//
#define AIC_STRIKE_WEAPONS_FREE          0x0000040CL

// Not Translated
//
// MessageId: AIC_STRIKE_WEAPONS_TIGHT
//
// MessageText:
//
//  Strike, Weapons Tight repeat weapons tight%0
//
#define AIC_STRIKE_WEAPONS_TIGHT         0x0000040DL

// Not Translated
//
// MessageId: AIC_STRIKE_WEAPONS_SAFE
//
// MessageText:
//
//  Strike, Weapons Safe repeat weapons safe%0
//
#define AIC_STRIKE_WEAPONS_SAFE          0x0000040EL

// Not Translated
//
// MessageId: AIC_STRIKE_WEAPONS_HOLD
//
// MessageText:
//
//  Strike, Weapons Hold repeat weapons hold%0
//
#define AIC_STRIKE_WEAPONS_HOLD          0x0000040FL

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_STRIKE_HOTDOG
//
// MessageText:
//
//  %1 Hotdog%0
//
#define AIC_STRIKE_HOTDOG                0x00000410L

//
// %1 CAP callsign
// Not Translated
//
// MessageId: AIC_STRIKE_CAP_MAINTAIN
//
// MessageText:
//
//  %1 maintain current station%0
//
#define AIC_STRIKE_CAP_MAINTAIN          0x00000411L

//
// %1 CAP callsign
//
// %2 bearing
//
// %3 range
//
// %4 altitude 2
//
// %5 aspect
//
// %6 speed
// Not Translated
//
// MessageId: AIC_STRIKE_CAP_INTERCEPT_VECTOR
//
// MessageText:
//
//  %1 hostiles inbound, intercept vector %2, %3, %4, %5, %6.%0
//
#define AIC_STRIKE_CAP_INTERCEPT_VECTOR  0x00000412L

//
// %1 CAP callsign
//
// %2 bearing
//
// %3 range
//
// %4 altitude 2
//
// %5 aspect
//
// %6 speed
// Not Translated
//
// MessageId: AIC_STRIKE_CAP_INTERCEPT_BOGIES
//
// MessageText:
//
//  %1 intercept bogies %2, %3, %4, %5, %6.%0
//
#define AIC_STRIKE_CAP_INTERCEPT_BOGIES  0x00000413L

//
// %1 CAP callsign
//
// %2 bearing
//
// %3 range
//
// %4 altitude 2
//
// %5 aspect
//
// %6 speed
// Not Translated
//
// MessageId: AIC_STRIKE_CAP_INTERCEPT_ID_BOGIES
//
// MessageText:
//
//  %1 intercept and ID bogies %2, %3, %4, %5, %6.%0
//
#define AIC_STRIKE_CAP_INTERCEPT_ID_BOGIES 0x00000414L

//
// %1 CAP callsign
//
// %2 bearing
//
// %3 range
//
// %4 altitude 2
// Not Translated
//
// MessageId: AIC_STRIKE_CAP_AID
//
// MessageText:
//
//  %1 friendlies under hostile attack, bearing %2, %3, %4%0
//
#define AIC_STRIKE_CAP_AID               0x00000415L

//
// %1 CAP callsign
// Not Translated
//
// MessageId: AIC_STRIKE_CAP_BUSTER
//
// MessageText:
//
//  %1 buster%0
//
#define AIC_STRIKE_CAP_BUSTER            0x00000416L

//
// %1 CAP callsign
//
// %2 cardinal
//
// %3 range
// Not Translated
//
// MessageId: AIC_STRIKE_CAP_MARSHAL_SANDY
//
// MessageText:
//
//  %1 marshal with Sandy at %2 bullseye, %3%0
//
#define AIC_STRIKE_CAP_MARSHAL_SANDY     0x00000417L

//
// %1 CAP callsign
//
// %2 cardinal
//
// %3 range
// Not Translated
//
// MessageId: AIC_STRIKE_CAP_ANCHOR
//
// MessageText:
//
//  %1 anchor at %2 bullseye %3%0
//
#define AIC_STRIKE_CAP_ANCHOR            0x00000418L

//
// %1 CAP callsign
// Not Translated
//
// MessageId: AIC_STRIKE_CAP_BUSTER_HOME
//
// MessageText:
//
//  %1, Mother has hostiles inbound, buster to homeplate%0
//
#define AIC_STRIKE_CAP_BUSTER_HOME       0x00000419L

//
// %1 cardinal
//
// %2 range
// Not Translated
//
// MessageId: AIC_STRIKE_SANDY_SCRAMBLE
//
// MessageText:
//
//  Sandy, scramble, Friendly down at %1 bullseye, %2%0
//
#define AIC_STRIKE_SANDY_SCRAMBLE        0x0000041AL

//
// %1 cardinal
//
// %2 range
// Not Translated
//
// MessageId: AIC_STRIKE_SANDY_AWAIT_SARCAP
//
// MessageText:
//
//  Sandy, await SARCAP at %1 bullseye, %2%0
//
#define AIC_STRIKE_SANDY_AWAIT_SARCAP    0x0000041BL

//
// %1 cardinal
//
// %2 range
// Not Translated
//
// MessageId: AIC_STRIKE_SANDY_CLEARED_IN
//
// MessageText:
//
//  Sandy, cleared for ingress %1 bullseye, %2%0
//
#define AIC_STRIKE_SANDY_CLEARED_IN      0x0000041CL

// Not Translated
//
// MessageId: AIC_SANDY_COPY
//
// MessageText:
//
//  Sandy, copy%0
//
#define AIC_SANDY_COPY                   0x0000041DL

// Not Translated
//
// MessageId: AIC_SANDY_GOOD_WORK
//
// MessageText:
//
//  Good work Sandy!%0
//
#define AIC_SANDY_GOOD_WORK              0x0000041EL

// Not Translated
//
// MessageId: AIC_SANDY_UNDERSTOOD_NEG
//
// MessageText:
//
//  Sandy, understood negative contact.  Return to homeplate%0
//
#define AIC_SANDY_UNDERSTOOD_NEG         0x0000041FL

//
// %1 lead callsign/wingman number
// Not Translated
//
// MessageId: AIC_STRIKE_SAY_AGAIN
//
// MessageText:
//
//  %1 say again.%0
//
#define AIC_STRIKE_SAY_AGAIN             0x00000420L

//
// %1 lead callsign/wingman number
// Not Translated
//
// MessageId: AIC_STRIKE_REPEAT_LAST
//
// MessageText:
//
//  %1 repeat last transmission.%0
//
#define AIC_STRIKE_REPEAT_LAST           0x00000421L

//
// %1 Player callsign
//
// %2 bearing
//
// %3 range
// Not Translated
//
// MessageId: AIC_BULLSEYE_BEARING
//
// MessageText:
//
//  %1 bullseye bearing %2, %3%0
//
#define AIC_BULLSEYE_BEARING             0x00000422L

//
// %1 Player callsign
//
// %2 bearing
//
// %3 range
// Not Translated
//
// MessageId: AIC_IP_BEARING
//
// MessageText:
//
//  %1 IP  bearing %2, %3%0
//
#define AIC_IP_BEARING                   0x00000423L

//
// %1 Player callsign
//
// %2 bearing
//
// %3 range
// Not Translated
//
// MessageId: AIC_TARGET_BEARING
//
// MessageText:
//
//  %1 target  bearing %2, %3%0
//
#define AIC_TARGET_BEARING               0x00000424L

//
// %1 Player callsign
//
// %2 bearing
//
// %3 range
// Not Translated
//
// MessageId: AIC_HOME_BEARING
//
// MessageText:
//
//  %1 homeplate  bearing %2, %3%0
//
#define AIC_HOME_BEARING                 0x00000425L

//
// %1 Player callsign
//
// %2 bearing
//
// %3 range
//
// %4 altitude 2
// Not Translated
//
// MessageId: AIC_TANKER_BEARING
//
// MessageText:
//
//  %1 tanker bearing %2, %3, %4%0
//
#define AIC_TANKER_BEARING               0x00000426L

//
// %1 Player callsign
//
// %2 bearing
//
// %3 range
// Not Translated
//
// MessageId: AIC_DIVERT_BEARING
//
// MessageText:
//
//  %1 divert  bearing %2, %3%0
//
#define AIC_DIVERT_BEARING               0x00000427L

//
// %1 Player callsign
//
// %2 bearing
//
// %3 range
//
// %4 altitude 2
// Not Translated
//
// MessageId: AIC_PACKAGE_BEARING
//
// MessageText:
//
//  %1 package bearing %2, %3, %4%0
//
#define AIC_PACKAGE_BEARING              0x00000428L

//
// %1 callsign
//
// %2 contact name
//
// %3 <group/single>
// Not Translated
//
// MessageId: AIC_STRIKE_COMMIT
//
// MessageText:
//
//  %1 commit, %2 %3%0
//
#define AIC_STRIKE_COMMIT                0x00000429L

//
// %1 callsign
//
// %2 contact name
//
// %3 <group/single>
// Not Translated
//
// MessageId: AIC_STRIKE_NEG_COMMIT
//
// MessageText:
//
//  %1 negative commit, %2 %3%0
//
#define AIC_STRIKE_NEG_COMMIT            0x0000042AL

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_GREAT_JOB
//
// MessageText:
//
//  %1 Great job!%0
//
#define AIC_GREAT_JOB                    0x0000042BL

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_GOOD_WORK
//
// MessageText:
//
//  %1 Good work!%0
//
#define AIC_GOOD_WORK                    0x0000042CL

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_OUTSTANDING
//
// MessageText:
//
//  %1 Outstanding!%0
//
#define AIC_OUTSTANDING                  0x0000042DL

//
// %1 Player flight
// Not Translated
//
// MessageId: AIC_MISSION_COMPLETE
//
// MessageText:
//
//  %1 Mission complete%0
//
#define AIC_MISSION_COMPLETE             0x0000042EL

//
// %1 Player flight
// Not Translated
//
// MessageId: AIC_MISSION_COMPLETE_HOME
//
// MessageText:
//
//  %1 Mission complete, return to homeplate%0
//
#define AIC_MISSION_COMPLETE_HOME        0x0000042FL

//
// %1 Player flight
// Not Translated
//
// MessageId: AIC_TIME_TO_COME_HOME
//
// MessageText:
//
//  %1 Time to come home%0
//
#define AIC_TIME_TO_COME_HOME            0x00000430L

//
// %1 Player flight
//
// MessageId: AIC_WING_COMPLETE_HOME
//
// MessageText:
//
//  %1 Mission complete, let's go home%0
//
#define AIC_WING_COMPLETE_HOME           0x00000431L

//
// %1 Player flight
//
// MessageId: AIC_WING_TIME_GO_HOME
//
// MessageText:
//
//  %1, Time to go home.%0
//
#define AIC_WING_TIME_GO_HOME            0x00000432L

// Not Translated
//
// MessageId: AIC_HOT
//
// MessageText:
//
//  Hot%0
//
#define AIC_HOT                          0x00000433L

// Not Translated
//
// MessageId: AIC_COLD
//
// MessageText:
//
//  Cold%0
//
#define AIC_COLD                         0x00000434L

// Not Translated
//
// MessageId: AIC_VERY_HIGH
//
// MessageText:
//
//  very high%0
//
#define AIC_VERY_HIGH                    0x00000435L

//
// %1 count
// Not Translated
//
// MessageId: AIC_THOUSAND
//
// MessageText:
//
//  %1 thousand%0
//
#define AIC_THOUSAND                     0x00000436L

// Not Translated
//
// MessageId: AIC_FIRST
//
// MessageText:
//
//  First%0
//
#define AIC_FIRST                        0x00000437L

// Not Translated
//
// MessageId: AIC_SECOND
//
// MessageText:
//
//  Second%0
//
#define AIC_SECOND                       0x00000438L

// Not Translated
//
// MessageId: AIC_THIRD
//
// MessageText:
//
//  Third%0
//
#define AIC_THIRD                        0x00000439L

// Not Translated
//
// MessageId: AIC_FOURTH
//
// MessageText:
//
//  Fourth%0
//
#define AIC_FOURTH                       0x0000043AL

// Not Translated
//
// MessageId: AIC_FIFTH
//
// MessageText:
//
//  Fifth%0
//
#define AIC_FIFTH                        0x0000043BL

// Not Translated
//
// MessageId: AIC_SIXTH
//
// MessageText:
//
//  Sixth%0
//
#define AIC_SIXTH                        0x0000043CL

// Not Translated
//
// MessageId: AIC_SEVENTH
//
// MessageText:
//
//  Seventh%0
//
#define AIC_SEVENTH                      0x0000043DL

// Not Translated
//
// MessageId: AIC_EIGHTH
//
// MessageText:
//
//  Eighth%0
//
#define AIC_EIGHTH                       0x0000043EL

// Not Translated
//
// MessageId: AIC_NINTH
//
// MessageText:
//
//  Ninth%0
//
#define AIC_NINTH                        0x0000043FL

// Not Translated
//
// MessageId: AIC_TENTH
//
// MessageText:
//
//  Tenth%0
//
#define AIC_TENTH                        0x00000440L

// Not Translated
//
// MessageId: AIC_ELEVENTH
//
// MessageText:
//
//  Eleventh%0
//
#define AIC_ELEVENTH                     0x00000441L

// Not Translated
//
// MessageId: AIC_TWELTH
//
// MessageText:
//
//  Twelth%0
//
#define AIC_TWELTH                       0x00000442L

// Not Translated
//
// MessageId: AIC_THIRTEENTH
//
// MessageText:
//
//  Thirteenth%0
//
#define AIC_THIRTEENTH                   0x00000443L

// Not Translated
//
// MessageId: AIC_FOURTEENTH
//
// MessageText:
//
//  Fourteenth%0
//
#define AIC_FOURTEENTH                   0x00000444L

// Not Translated
//
// MessageId: AIC_FIFTEENTH
//
// MessageText:
//
//  Fifteenth%0
//
#define AIC_FIFTEENTH                    0x00000445L

// Not Translated
//
// MessageId: AIC_SIXTEENTH
//
// MessageText:
//
//  Sixteenth%0
//
#define AIC_SIXTEENTH                    0x00000446L

// Not Translated
//
// MessageId: AIC_SEVENTEENTH
//
// MessageText:
//
//  Seventeenth%0
//
#define AIC_SEVENTEENTH                  0x00000447L

// Not Translated
//
// MessageId: AIC_EIGHTEENTH
//
// MessageText:
//
//  Eighteenth%0
//
#define AIC_EIGHTEENTH                   0x00000448L

// Not Translated
//
// MessageId: AIC_NINTEENTH
//
// MessageText:
//
//  Nineteenth%0
//
#define AIC_NINTEENTH                    0x00000449L

// Not Translated
//
// MessageId: AIC_TWENTIETH
//
// MessageText:
//
//  Twentieth%0
//
#define AIC_TWENTIETH                    0x0000044AL

// Not Translated
//
// MessageId: AIC_NEAR
//
// MessageText:
//
//  Near%0
//
#define AIC_NEAR                         0x0000044BL

// Not Translated
//
// MessageId: AIC_FAR
//
// MessageText:
//
//  Far%0
//
#define AIC_FAR                          0x0000044CL

// Not Translated
//
// MessageId: AIC_RANGE
//
// MessageText:
//
//  range%0
//
#define AIC_RANGE                        0x0000044DL

// Not Translated
//
// MessageId: AIC_AZIMUTH
//
// MessageText:
//
//  azimuth%0
//
#define AIC_AZIMUTH                      0x0000044EL

// Not Translated
//
// MessageId: AIC_ECHELON
//
// MessageText:
//
//  echelon%0
//
#define AIC_ECHELON                      0x0000044FL

// Not Translated
//
// MessageId: AIC_VIC
//
// MessageText:
//
//  vic%0
//
#define AIC_VIC                          0x00000450L

// Not Translated
//
// MessageId: AIC_STACK
//
// MessageText:
//
//  stack%0
//
#define AIC_STACK                        0x00000451L

// Not Translated
//
// MessageId: AIC_CHAMPAGNE
//
// MessageText:
//
//  champagne%0
//
#define AIC_CHAMPAGNE                    0x00000452L

// Not Translated
//
// MessageId: AIC_WALL
//
// MessageText:
//
//  wall%0
//
#define AIC_WALL                         0x00000453L

// Not Translated
//
// MessageId: AIC_LADDER
//
// MessageText:
//
//  ladder%0
//
#define AIC_LADDER                       0x00000454L

// Not Translated
//
// MessageId: AIC_BOX
//
// MessageText:
//
//  box%0
//
#define AIC_BOX                          0x00000455L

// Not Translated
//
// MessageId: AIC_CAPPING
//
// MessageText:
//
//  capping%0
//
#define AIC_CAPPING                      0x00000456L

// Not Translated
//
// MessageId: AIC_GORILLA
//
// MessageText:
//
//  gorilla%0
//
#define AIC_GORILLA                      0x00000457L

// Not Translated
//
// MessageId: AIC_HEAVY
//
// MessageText:
//
//  heavy%0
//
#define AIC_HEAVY                        0x00000458L

// Not Translated
//
// MessageId: AIC_CANDID
//
// MessageText:
//
//  Candid%0
//
#define AIC_CANDID                       0x00000459L

// Not Translated
//
// MessageId: AIC_MAINSTAY
//
// MessageText:
//
//  Mainstay%0
//
#define AIC_MAINSTAY                     0x0000045AL

// Not Translated
//
// MessageId: AIC_HELIX
//
// MessageText:
//
//  Helix%0
//
#define AIC_HELIX                        0x0000045BL

// Not Translated
//
// MessageId: AIC_HOKUM
//
// MessageText:
//
//  Hokum%0
//
#define AIC_HOKUM                        0x0000045CL

// Not Translated
//
// MessageId: AIC_FISHBED
//
// MessageText:
//
//  Fishbed%0
//
#define AIC_FISHBED                      0x0000045DL

// Not Translated
//
// MessageId: AIC_FLOGGER
//
// MessageText:
//
//  Flogger%0
//
#define AIC_FLOGGER                      0x0000045EL

// Not Translated
//
// MessageId: AIC_FOXBAT
//
// MessageText:
//
//  Foxbat%0
//
#define AIC_FOXBAT                       0x0000045FL

// Not Translated
//
// MessageId: AIC_FULCRUM
//
// MessageText:
//
//  Fulcrum%0
//
#define AIC_FULCRUM                      0x00000460L

// Not Translated
//
// MessageId: AIC_FOXHOUND
//
// MessageText:
//
//  Foxhound%0
//
#define AIC_FOXHOUND                     0x00000461L

// Not Translated
//
// MessageId: AIC_HIP
//
// MessageText:
//
//  Hip%0
//
#define AIC_HIP                          0x00000462L

// Not Translated
//
// MessageId: AIC_HIND
//
// MessageText:
//
//  Hind%0
//
#define AIC_HIND                         0x00000463L

// Not Translated
//
// MessageId: AIC_HAVOC
//
// MessageText:
//
//  Havoc%0
//
#define AIC_HAVOC                        0x00000464L

// Not Translated
//
// MessageId: AIC_FITTER
//
// MessageText:
//
//  Fitter%0
//
#define AIC_FITTER                       0x00000465L

// Not Translated
//
// MessageId: AIC_FENCER
//
// MessageText:
//
//  Fencer%0
//
#define AIC_FENCER                       0x00000466L

// Not Translated
//
// MessageId: AIC_FROGFOOT
//
// MessageText:
//
//  Frogfoot%0
//
#define AIC_FROGFOOT                     0x00000467L

// Not Translated
//
// MessageId: AIC_FLANKER
//
// MessageText:
//
//  Flanker%0
//
#define AIC_FLANKER                      0x00000468L

// Not Translated
//
// MessageId: AIC_BEAR
//
// MessageText:
//
//  Bear%0
//
#define AIC_BEAR                         0x00000469L

// Not Translated
//
// MessageId: AIC_BLINDER
//
// MessageText:
//
//  Blinder%0
//
#define AIC_BLINDER                      0x0000046AL

// Not Translated
//
// MessageId: AIC_BACKFIRE
//
// MessageText:
//
//  Backfire%0
//
#define AIC_BACKFIRE                     0x0000046BL

// Not Translated
//
// MessageId: AIC_CHICK
//
// MessageText:
//
//  Chick%0
//
#define AIC_CHICK                        0x0000046CL

// Not Translated
//
// MessageId: AIC_HOSTILE
//
// MessageText:
//
//  Hostile%0
//
#define AIC_HOSTILE                      0x0000046DL

// Not Translated
//
// MessageId: AIC_BOGEY
//
// MessageText:
//
//  Bogey%0
//
#define AIC_BOGEY                        0x0000046EL

// Not Translated
//
// MessageId: AIC_UNABLE_CLEAN
//
// MessageText:
//
//  Unable, clean%0
//
#define AIC_UNABLE_CLEAN                 0x0000046FL

// Not Translated
//
// MessageId: AIC_FAST
//
// MessageText:
//
//  Fast%0
//
#define AIC_FAST                         0x00000470L

// Not Translated
//
// MessageId: AIC_SLOW
//
// MessageText:
//
//  Slow%0
//
#define AIC_SLOW                         0x00000471L

//
// %1 Ship callsign <Strike>
//
// %2 callsign or modex
//
// %3 radial
//
// %4 range
//
// %5 altitude 3
//
// %6 fuel state
// Not Translated
//
// MessageId: AIC_TELL_STRIKE_INBOUND
//
// MessageText:
//
//  %1, %2, inbound Mother's %3, %4, %5, %6%0
//
#define AIC_TELL_STRIKE_INBOUND          0x00000472L

//
// %1 callsign or modex
//
// %2 altitude 3
//
// %3 fuel state
// Not Translated
//
// MessageId: AIC_ESTABLISHED_MARSHAL
//
// MessageText:
//
//  %1 established %2, %3%0
//
#define AIC_ESTABLISHED_MARSHAL          0x00000473L

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_COMMENCING_PUSH
//
// MessageText:
//
//  %1 commencing %2%0
//
#define AIC_COMMENCING_PUSH              0x00000474L

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_PLATFORM
//
// MessageText:
//
//  %1 platform, %2%0
//
#define AIC_PLATFORM                     0x00000475L

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_IS_UP
//
// MessageText:
//
//  %1 is up, %2%0
//
#define AIC_IS_UP                        0x00000476L

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_HORNET_BALL
//
// MessageText:
//
//  %1 hornet ball, %2%0
//
#define AIC_HORNET_BALL                  0x00000477L

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_S_HORNET_BALL
//
// MessageText:
//
//  %1 super hornet ball, %2%0
//
#define AIC_S_HORNET_BALL                0x00000478L

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_PROWLER_BALL
//
// MessageText:
//
//  %1 prowler ball, %2%0
//
#define AIC_PROWLER_BALL                 0x00000479L

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_TOMCAT_BALL
//
// MessageText:
//
//  %1 tomcat ball, %2%0
//
#define AIC_TOMCAT_BALL                  0x0000047AL

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_VIKING_BALL
//
// MessageText:
//
//  %1 viking ball, %2%0
//
#define AIC_VIKING_BALL                  0x0000047BL

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_HAWKEYE_BALL
//
// MessageText:
//
//  %1 hawkeye ball, %2%0
//
#define AIC_HAWKEYE_BALL                 0x0000047CL

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_CLARA
//
// MessageText:
//
//  %1 clara %2%0
//
#define AIC_CLARA                        0x0000047DL

//
// %1 callsign or modex
//
// %2 fuel state
// Not Translated
//
// MessageId: AIC_ABEAM
//
// MessageText:
//
//  %1 abeam, %2%0
//
#define AIC_ABEAM                        0x0000047EL

//
// %1 callsign or modex
// Not Translated
//
// MessageId: AIC_ACLS_ON_GLIDESLOPE
//
// MessageText:
//
//  %1 on glideslope%0
//
#define AIC_ACLS_ON_GLIDESLOPE           0x0000047FL

//
// %1 callsign or modex
// Not Translated
//
// MessageId: AIC_ACLS_FLY_UP
//
// MessageText:
//
//  %1 fly up%0
//
#define AIC_ACLS_FLY_UP                  0x00000480L

//
// %1 callsign or modex
// Not Translated
//
// MessageId: AIC_ACLS_FLY_DOWN
//
// MessageText:
//
//  %1 fly down%0
//
#define AIC_ACLS_FLY_DOWN                0x00000481L

//
// %1 callsign or modex
// Not Translated
//
// MessageId: AIC_ACLS_ON_CENTERLINE
//
// MessageText:
//
//  %1 on centerline%0
//
#define AIC_ACLS_ON_CENTERLINE           0x00000482L

//
// %1 callsign or modex
// Not Translated
//
// MessageId: AIC_ACLS_FLY_LEFT
//
// MessageText:
//
//  %1 fly left%0
//
#define AIC_ACLS_FLY_LEFT                0x00000483L

//
// %1 callsign or modex
// Not Translated
//
// MessageId: AIC_ACLS_FLY_RIGHT
//
// MessageText:
//
//  %1 fly right%0
//
#define AIC_ACLS_FLY_RIGHT               0x00000484L

//
// %1 k lbs of fuel as X.X
// Not Translated
//
// MessageId: AIC_FUEL_STATE
//
// MessageText:
//
//  state %1%0
//
#define AIC_FUEL_STATE                   0x00000485L

//
// %1 callsign or modex
//
// %2 DME range
//
// %3 fuel state
// Not Translated
//
// MessageId: AIC_LANDING_DISTANCE
//
// MessageText:
//
//  %1 %2, %3%0
//
#define AIC_LANDING_DISTANCE             0x00000486L

// Not Translated
//
// MessageId: AIC_CHAT_ALLIED
//
// MessageText:
//
//  Chat Allied Flight%0
//
#define AIC_CHAT_ALLIED                  0x00000487L

// Not Translated
//
// MessageId: AIC_CHAT_ALL
//
// MessageText:
//
//  Chat All%0
//
#define AIC_CHAT_ALL                     0x00000488L

// Not Translated
//
// MessageId: AIC_ROGER_MSG
//
// MessageText:
//
//  Roger%0
//
#define AIC_ROGER_MSG                    0x00000489L

// Not Translated
//
// MessageId: AIC_NEGATIVE_MSG
//
// MessageText:
//
//  Negative%0
//
#define AIC_NEGATIVE_MSG                 0x0000048AL

// Not Translated
//
// MessageId: AIC_FEET_WET
//
// MessageText:
//
//  Feet Wet%0
//
#define AIC_FEET_WET                     0x0000048BL

// Not Translated
//
// MessageId: AIC_FEET_DRY
//
// MessageText:
//
//  Feet Dry%0
//
#define AIC_FEET_DRY                     0x0000048CL

//
// %1 Wingman callsign
//
// %2 clock position
// Not Translated
//
// MessageId: AIC_WING_SHIP_SPOT_1
//
// MessageText:
//
//  %1, surface ships at %2%0
//
#define AIC_WING_SHIP_SPOT_1             0x0000048DL

//
// %1 Wingman callsign
//
// %2 clock position
// Not Translated
//
// MessageId: AIC_WING_SKUNKS_SPOT_1
//
// MessageText:
//
//  %1, skunks at %2%0
//
#define AIC_WING_SKUNKS_SPOT_1           0x0000048EL

//
// %1 Wingman callsign
//
// %2 clock position
// Not Translated
//
// MessageId: AIC_WING_SKUNK_SPOT_1
//
// MessageText:
//
//  %1, skunk at %2%0
//
#define AIC_WING_SKUNK_SPOT_1            0x0000048FL

//
// %1 Wingman callsign
//
// %2 clock position
// Not Translated
//
// MessageId: AIC_WING_GRND_TRG_SPOT_1
//
// MessageText:
//
//  %1, ground targets at %2%0
//
#define AIC_WING_GRND_TRG_SPOT_1         0x00000490L

//
// %1 Wingman callsign
//
// %2 clock position
// Not Translated
//
// MessageId: AIC_WING_AAA_SPOT_3
//
// MessageText:
//
//  %1, guns below us at %2%0
//
#define AIC_WING_AAA_SPOT_3              0x00000491L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_AAA_SPOT_4
//
// MessageText:
//
//  %1, Triple A fire spotted%0
//
#define AIC_WING_AAA_SPOT_4              0x00000492L

//
// %1 Wingman callsign
//
// %2 position
// Not Translated
//
// MessageId: AIC_WING_AAA_SPOT_RAF_1
//
// MessageText:
//
//  %1 flak at %2%0
//
#define AIC_WING_AAA_SPOT_RAF_1          0x00000493L

//
// %1 Wingman callsign
//
// %2 clock position
// Not Translated
//
// MessageId: AIC_WING_SAM_SPOT_4
//
// MessageText:
//
//  %1, Mud Launch!  %2%0
//
#define AIC_WING_SAM_SPOT_4              0x00000494L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_DEFENSIVE_AAA
//
// MessageText:
//
//  %1 is engaged Triple A%0
//
#define AIC_WING_DEFENSIVE_AAA           0x00000495L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_DEFENSIVE_ALAMO
//
// MessageText:
//
//  %1 is engaged defensive ALAMO%0
//
#define AIC_WING_DEFENSIVE_ALAMO         0x00000496L

//
// %1 Wingman callsign
//
// %2 Clock position
//
// MessageId: AIC_WING_INCOMING_MISSILE
//
// MessageText:
//
//  %1 Incoming missile, %2%0
//
#define AIC_WING_INCOMING_MISSILE        0x00000497L

//
// %1 Wingman callsign
//
// %2 Clock position
//
// MessageId: AIC_WING_MISSILE_INBOUND
//
// MessageText:
//
//  %1 Missile inbound, %2%0
//
#define AIC_WING_MISSILE_INBOUND         0x00000498L

//
// %1Flight callsign
//
// %2 Clock position
//
// MessageId: AIC_WING_HEADS_UP_MISSILE
//
// MessageText:
//
//  %1 Heads up!  Missile Launch, %2!%0
//
#define AIC_WING_HEADS_UP_MISSILE        0x00000499L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_CHAFF
//
// MessageText:
//
//  %1 Chaff!  Chaff!%0
//
#define AIC_WING_CHAFF                   0x0000049AL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_FLARE
//
// MessageText:
//
//  %1 Flare!  Flare!%0
//
#define AIC_WING_FLARE                   0x0000049BL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_MUSIC_ON
//
// MessageText:
//
//  %1 Music On%0
//
#define AIC_WING_MUSIC_ON                0x0000049CL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_MUSIC_OFF
//
// MessageText:
//
//  %1 Music Off%0
//
#define AIC_WING_MUSIC_OFF               0x0000049DL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_GOMERS
//
// MessageText:
//
//  %1, gomers spotted, attacking%0
//
#define AIC_WING_ATTACKING_GOMERS        0x0000049EL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_ARMOR
//
// MessageText:
//
//  %1, enemy armor in sight, in hot.%0
//
#define AIC_WING_ATTACKING_ARMOR         0x0000049FL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_DUG_GUNS
//
// MessageText:
//
//  %1, dug in guns spotted, rolling in hot.%0
//
#define AIC_WING_ATTACKING_DUG_GUNS      0x000004A0L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_TRUCKS
//
// MessageText:
//
//  %1, trucks in sight, commencing attack.%0
//
#define AIC_WING_ATTACKING_TRUCKS        0x000004A1L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_AAA
//
// MessageText:
//
//  %1, triple A spotted, attacking%0
//
#define AIC_WING_ATTACKING_AAA           0x000004A2L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_DUG_SAM
//
// MessageText:
//
//  %1, dug in SAM site in sight, commencing attack.%0
//
#define AIC_WING_ATTACKING_DUG_SAM       0x000004A3L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_MOBILE_SAM
//
// MessageText:
//
//  %1, moblie SAM site spotted, rolling in hot%0
//
#define AIC_WING_ATTACKING_MOBILE_SAM    0x000004A4L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_SHIP
//
// MessageText:
//
//  %1, enemy ship in sight, attacking%0
//
#define AIC_WING_ATTACKING_SHIP          0x000004A5L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_CAN
//
// MessageText:
//
//  %1, enemy tin can spotted, attacking.%0
//
#define AIC_WING_ATTACKING_CAN           0x000004A6L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_CONTACT
//
// MessageText:
//
//  %1, target contact, in hot.%0
//
#define AIC_WING_ATTACKING_CONTACT       0x000004A7L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_CAPTURED
//
// MessageText:
//
//  %1, target captured, in hot.%0
//
#define AIC_WING_ATTACKING_CAPTURED      0x000004A8L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_HITS_IN_AREA
//
// MessageText:
//
//  %1, hits in target area, additional targets remaining.%0
//
#define AIC_WING_HITS_IN_AREA            0x000004A9L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_HIT
//
// MessageText:
//
//  %1 I've been hit!%0
//
#define AIC_WING_HIT                     0x000004AAL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_HIT_HURT
//
// MessageText:
//
//  %1 I'm hurt, assessing damage now!%0
//
#define AIC_WING_HIT_HURT                0x000004ABL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_HIT_MAJOR
//
// MessageText:
//
//  %1 I took some major hits!%0
//
#define AIC_WING_HIT_MAJOR               0x000004ACL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_TAKING_AAA
//
// MessageText:
//
//  %1 I'm taking heavy triple A fire!%0
//
#define AIC_WING_TAKING_AAA              0x000004ADL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_GET_ABOVE
//
// MessageText:
//
//  %1 I need to get above this stuff!%0
//
#define AIC_WING_GET_ABOVE               0x000004AEL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_I_AM_HIT
//
// MessageText:
//
//  %1 I'm hit!%0
//
#define AIC_WING_I_AM_HIT                0x000004AFL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_TAKING_FIRE
//
// MessageText:
//
//  %1 I'm taking fire!%0
//
#define AIC_WING_TAKING_FIRE             0x000004B0L

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_TAKING_CANNON_FIRE
//
// MessageText:
//
//  %1 I'm taking cannon fire.%0
//
#define AIC_WING_TAKING_CANNON_FIRE      0x000004B1L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_MISSILE_JUST_HIT
//
// MessageText:
//
//  %1 that missile just hit me!%0
//
#define AIC_WING_MISSILE_JUST_HIT        0x000004B2L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_THINK_MISSILE_HIT
//
// MessageText:
//
//  %1 I think I just took a missile hit!%0
//
#define AIC_WING_THINK_MISSILE_HIT       0x000004B3L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_L_HIT
//
// MessageText:
//
//  %1 I've been hit!%0
//
#define AIC_WING_L_HIT                   0x000004B4L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_TICKLE_GCI
//
// MessageText:
//
//  %1 Tickle GCI%0
//
#define AIC_WING_TICKLE_GCI              0x000004B5L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_TICKLE_SAM
//
// MessageText:
//
//  %1 Tickle SAM%0
//
#define AIC_WING_TICKLE_SAM              0x000004B6L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_TICKLE
//
// MessageText:
//
//  %1 Tickle%0
//
#define AIC_TICKLE                       0x000004B7L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_RADAR_DETECT_10
//
// MessageText:
//
//  %1 Nails MiG %2%0
//
#define AIC_WING_RADAR_DETECT_10         0x000004B8L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_NAKED
//
// MessageText:
//
//  %1 naked%0
//
#define AIC_WING_NAKED                   0x000004B9L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_BLANK
//
// MessageText:
//
//  %1 Blank%0
//
#define AIC_WING_BLANK                   0x000004BAL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_EMPTY
//
// MessageText:
//
//  %1 Empty%0
//
#define AIC_WING_EMPTY                   0x000004BBL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_DUCKS_AWAY
//
// MessageText:
//
//  %1 ducks away!%0
//
#define AIC_WING_DUCKS_AWAY              0x000004BCL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_MAGNUM
//
// MessageText:
//
//  %1 Magnum!%0
//
#define AIC_WING_MAGNUM                  0x000004BDL

//
// %1 Wingman callsign
//
// %2 position
//
// %3 emitter type
// Not Translated
//
// MessageId: AIC_WING_MUD_SPIKE
//
// MessageText:
//
//  %1 mud spike, %2, %3%0
//
#define AIC_WING_MUD_SPIKE               0x000004BEL

//
// %1 Wingman callsign
//
// %2 position
//
// %3 emitter type
// Not Translated
//
// MessageId: AIC_WING_WORKING
//
// MessageText:
//
//  %1 working, %2, %3%0
//
#define AIC_WING_WORKING                 0x000004BFL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_ARIZONA
//
// MessageText:
//
//  %1 Arizona%0
//
#define AIC_WING_ARIZONA                 0x000004C0L

//
// %1 wingman number
//
// %2 size (group/single)
//
// %3 bearing
//
// %4 range (# miles)
//
// %5 altitude (high/med/low)
//
// MessageId: AIC_CAP_CONTACT_1
//
// MessageText:
//
//  %1, contact, %2, %3, %4, %5%0
//
#define AIC_CAP_CONTACT_1                0x000004C1L

//
// %1 Flight callsign
// Not Translated
//
// MessageId: AIC_ON_STATION
//
// MessageText:
//
//  %1 on station%0
//
#define AIC_ON_STATION                   0x000004C2L

//
// %1 Flight callsign
// Not Translated
//
// MessageId: AIC_BEGINNING_SWEEP
//
// MessageText:
//
//  %1 we're beginning our sweep of the target area now%0
//
#define AIC_BEGINNING_SWEEP              0x000004C3L

//
// %1 Flight callsign
// Not Translated
//
// MessageId: AIC_SORT
//
// MessageText:
//
//  %1 sort%0
//
#define AIC_SORT                         0x000004C4L

//
// %1 wingman number
//
// %2 cardinal2
//
// %3 bandit type
// Not Translated
//
// MessageId: AIC_PRINT
//
// MessageText:
//
//  %1, print %2 %3.%0
//
#define AIC_PRINT                        0x000004C5L

//
// %1 callsign
// Not Translated
//
// MessageId: AIC_FADED
//
// MessageText:
//
//  %1 faded%0
//
#define AIC_FADED                        0x000004C6L

//
// %1 callsign
// Not Translated
//
// MessageId: AIC_NEW_PICTURE
//
// MessageText:
//
//  %1 new picture%0
//
#define AIC_NEW_PICTURE                  0x000004C7L

//
// %1 lead/wingman callsign
//
// %2 range
//
// %3 bearing
//
// %4 altitude
// Not Translated
//
// MessageId: AIC_RAYGUN
//
// MessageText:
//
//  %1 raygun, %2, bullseye, %3, %4%0
//
#define AIC_RAYGUN                       0x000004C8L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_STROBE_DIR
//
// MessageText:
//
//  %1 strobe %2%0
//
#define AIC_WING_STROBE_DIR              0x000004C9L

//
// %1 Wingman number
//
// %2 position
//
// MessageId: AIC_WING_MUSIC_DIR
//
// MessageText:
//
//  %1 music %2%0
//
#define AIC_WING_MUSIC_DIR               0x000004CAL

//
// %1 Wingman number
//
// MessageId: AIC_WING_BURNTHROUGH
//
// MessageText:
//
//  %1 burnthrough%0
//
#define AIC_WING_BURNTHROUGH             0x000004CBL

//
// %1 callsign
//
// MessageId: AIC_WING_GO_BROADCAST
//
// MessageText:
//
//  %1 go broadcast%0
//
#define AIC_WING_GO_BROADCAST            0x000004CCL

//
// %1 callsign
//
// MessageId: AIC_WING_GO_TACTICAL
//
// MessageText:
//
//  %1 go tactical%0
//
#define AIC_WING_GO_TACTICAL             0x000004CDL

//
// %1 range
//
// %2 cardinal
//
// %3 altitude
//
// %4 cardinal 
// Not Translated
//
// MessageId: AIC_DO_BULLSEYE
//
// MessageText:
//
//  %1 %2 bullseye, %3, tracking %4.%0
//
#define AIC_DO_BULLSEYE                  0x000004CEL

//
// %1 range
//
// %2 bearing
//
// %3 altitude
//
// %4 cardinal 
// Not Translated
//
// MessageId: AIC_DO_DIGIT_BULLSEYE
//
// MessageText:
//
//  bullseye %2, %1, %3, tracking %4.%0
//
#define AIC_DO_DIGIT_BULLSEYE            0x000004CFL

//
// %1 bearing
//
// %2 range
//
// %3 altitude2
//
// %4 aspect
// Not Translated
//
// MessageId: AIC_DO_BRA
//
// MessageText:
//
//  bra %1, %2, %3, %4%0
//
#define AIC_DO_BRA                       0x000004D0L

// Not Translated
//
// MessageId: AIC_BULLSEYE
//
// MessageText:
//
//  bullseye%0
//
#define AIC_BULLSEYE                     0x000004D1L

// Not Translated
//
// MessageId: AIC_TRACKING
//
// MessageText:
//
//  tracking%0
//
#define AIC_TRACKING                     0x000004D2L

// Not Translated
//
// MessageId: AIC_BRA
//
// MessageText:
//
//  bra%0
//
#define AIC_BRA                          0x000004D3L

//
// %1 wingman number
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_LONE_GROUP
//
// MessageText:
//
//  %1 lone group %2%0
//
#define AIC_LONE_GROUP                   0x000004D4L

//
// %1 wingman number
//
// %2 number of groups
// Not Translated
//
// MessageId: AIC_GROUPS
//
// MessageText:
//
//  %1 %2 groups%0
//
#define AIC_GROUPS                       0x000004D5L

//
// %1 order (group number)
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_GROUP_INFO
//
// MessageText:
//
//  %1 group, %2%0
//
#define AIC_GROUP_INFO                   0x000004D6L

//
// %1 wingman number
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_POPUP_GROUP
//
// MessageText:
//
//  %1 popup group %2%0
//
#define AIC_POPUP_GROUP                  0x000004D7L

//
// %1 wingman number
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_LONE_SINGLE
//
// MessageText:
//
//  %1 lone single %2%0
//
#define AIC_LONE_SINGLE                  0x000004D8L

//
// %1 wingman number
//
// %2 number of groups
// Not Translated
//
// MessageId: AIC_SINGLES
//
// MessageText:
//
//  %1 %2 singles%0
//
#define AIC_SINGLES                      0x000004D9L

//
// %1 order (single number)
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_SINGLE_INFO
//
// MessageText:
//
//  %1 single, %2%0
//
#define AIC_SINGLE_INFO                  0x000004DAL

//
// %1 wingman number
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_POPUP_SINGLE
//
// MessageText:
//
//  %1 popup single %2%0
//
#define AIC_POPUP_SINGLE                 0x000004DBL

//
// %1 wingman callsign
//
// %2 contact name
//
// %3 group
// Not Translated
//
// MessageId: AIC_TARGET
//
// MessageText:
//
//  %1 target %2 %3%0
//
#define AIC_TARGET                       0x000004DCL

//
// %1 Flight callsign
// Not Translated
//
// MessageId: AIC_FLIGHT_ENGAGE_BANDITS
//
// MessageText:
//
//  %1 engage bandits%0
//
#define AIC_FLIGHT_ENGAGE_BANDITS        0x000004DDL

//
// %1 Flight callsign
// Not Translated
//
// MessageId: AIC_FLIGHT_WEAPONS_FREE
//
// MessageText:
//
//  %1 weapons free%0
//
#define AIC_FLIGHT_WEAPONS_FREE          0x000004DEL

//
// %1 Flight callsign
// Not Translated
//
// MessageId: AIC_FLIGHT_CLEARED_HOT
//
// MessageText:
//
//  %1 cleared hot%0
//
#define AIC_FLIGHT_CLEARED_HOT           0x000004DFL

//
// %1 Flight callsign
// Not Translated
//
// MessageId: AIC_FLIGHT_COMMENCE_ATTACK
//
// MessageText:
//
//  %1 commence attack%0
//
#define AIC_FLIGHT_COMMENCE_ATTACK       0x000004E0L

//
// %1 Wingman number
//  
//
// MessageId: AIC_ENGAGE_READY_MSG_3
//
// MessageText:
//
//  %1 ready to engage bandits.%0
//
#define AIC_ENGAGE_READY_MSG_3           0x000004E1L

//
// %1 fighter callsign
//
// %2 enemy ID
// Not Translated
//
// MessageId: AIC_CAP_TALLY_BANDITS_ID
//
// MessageText:
//
//  %1, Tally bandits, I D %3%0
//
#define AIC_CAP_TALLY_BANDITS_ID         0x000004E2L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_INTERCEPT_BANDIT
//
// MessageText:
//
//  %1, Intercepting incoming bandit%0
//
#define AIC_CAP_INTERCEPT_BANDIT         0x000004E3L

//
// %1 fighter callsign
//
// MessageId: AIC_CAP_INTERCEPT_BANDITS
//
// MessageText:
//
//  %1, Intercepting incoming bandits%0
//
#define AIC_CAP_INTERCEPT_BANDITS        0x000004E4L

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_WING_PICTURE
//
// MessageText:
//
//  %1 picture%0
//
#define AIC_WING_PICTURE                 0x000004E5L

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_WING_BOGEY_DOPE
//
// MessageText:
//
//  %1 bogid dope%0
//
#define AIC_WING_BOGEY_DOPE              0x000004E6L

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_WING_JUDY
//
// MessageText:
//
//  %1 judy%0
//
#define AIC_WING_JUDY                    0x000004E7L

//
// %1 lead callsign
//
// %2 radar contact - group
//
// %3 range
//
// %4 altitude
//
// %5 cardinal
// Not Translated
//
// MessageId: AIC_DECLARE
//
// MessageText:
//
//  %1 %2 %3 bullseye, %3, %4, tracking %5%0
//
#define AIC_DECLARE                      0x000004E8L

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_ALPHA_BULLSEYE
//
// MessageText:
//
//  %1 Alpha check to bullseye%0
//
#define AIC_ALPHA_BULLSEYE               0x000004E9L

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_ALPHA_IP
//
// MessageText:
//
//  %1 Alpha check to I-P%0
//
#define AIC_ALPHA_IP                     0x000004EAL

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_ALPHA_TARGET
//
// MessageText:
//
//  %1 Alpha check to target%0
//
#define AIC_ALPHA_TARGET                 0x000004EBL

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_ALPHA_HOMEPLATE
//
// MessageText:
//
//  %1 Alpha check to homeplate%0
//
#define AIC_ALPHA_HOMEPLATE              0x000004ECL

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_ALPHA_TANKER
//
// MessageText:
//
//  %1 Alpha check to tanker%0
//
#define AIC_ALPHA_TANKER                 0x000004EDL

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_ALPHA_DIVERT
//
// MessageText:
//
//  %1 Alpha check to nearest divert%0
//
#define AIC_ALPHA_DIVERT                 0x000004EEL

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_ALPHA_PACKAGE
//
// MessageText:
//
//  %1 Alpha check to package%0
//
#define AIC_ALPHA_PACKAGE                0x000004EFL

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_TUMBLEWEED
//
// MessageText:
//
//  %1 tumbleweed%0
//
#define AIC_TUMBLEWEED                   0x000004F0L

//
// %1lead callsign
//
// %2 contact name
//
// %3 group/single
// Not Translated
//
// MessageId: AIC_REQUEST_COMMIT
//
// MessageText:
//
//  %1 request commit %2, %3%0
//
#define AIC_REQUEST_COMMIT               0x000004F1L

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_WING_REQUEST_SEAD
//
// MessageText:
//
//  %1 request SEAD support%0
//
#define AIC_WING_REQUEST_SEAD            0x000004F2L

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_WING_REQUEST_FIGHTER
//
// MessageText:
//
//  %1 request fighter support%0
//
#define AIC_WING_REQUEST_FIGHTER         0x000004F3L

//
// %1 lead callsign
// Not Translated
//
// MessageId: AIC_REQUEST_RELIEF
//
// MessageText:
//
//  %1 request relief%0
//
#define AIC_REQUEST_RELIEF               0x000004F4L

//
// %1 AWACS callsign
//
// %2 wignman callsign
//
// %3 range
//
// %4 cardinal
//
// MessageId: AIC_WSO_SAR_CALL_2
//
// MessageText:
//
//  %1, %2 has ejected, request immediate SAR scramble, %3 bullseye, %4.%0
//
#define AIC_WSO_SAR_CALL_2               0x000004F5L

//
// %1 wing callsign
//
// %2 Player callsign
//
// MessageId: AIC_WSO_HELP_ON_WAY_2
//
// MessageText:
//
//  %1, %2, understood, help is on the way%0
//
#define AIC_WSO_HELP_ON_WAY_2            0x000004F6L

//
// %1 tanker callsign
//
// %2 flight callsign
//
// %3 tanker gives
//
// MessageId: AIC_TANKER_LOOKING_FOR
//
// MessageText:
//
//  %1, %2, switches safe, nose cold, looking for %3.%0
//
#define AIC_TANKER_LOOKING_FOR           0x000004F7L

//
// %1 tanker callsign
//
// %2 flight callsign
//
// MessageId: AIC_TANKER_PRECONTACT
//
// MessageText:
//
//  %1, %2, precontact%0
//
#define AIC_TANKER_PRECONTACT            0x000004F8L

//
// %1 tanker callsign
//
// %2 flight callsign
//
// MessageId: AIC_TANKER_RECEIVING
//
// MessageText:
//
//  %1, %2, Receiving fuel%0
//
#define AIC_TANKER_RECEIVING             0x000004F9L

//
// %1 tanker callsign
//
// %2 flight callsign
//
// MessageId: AIC_TANKER_DISCONNECT
//
// MessageText:
//
//  %1, %2, Disconnect%0
//
#define AIC_TANKER_DISCONNECT            0x000004FAL

//
// %1 callsign of tanker
//
// %2 callsign of flight
//
// MessageId: AIC_WSO_TANKER_THANKS_MSG_2
//
// MessageText:
//
//  %1, %2, thanks for the gas.%0
//
#define AIC_WSO_TANKER_THANKS_MSG_2      0x000004FBL

//
// %1 count
// Not Translated
//
// MessageId: AIC_TANKER_GIVES
//
// MessageText:
//
//  %1 Kay%0
//
#define AIC_TANKER_GIVES                 0x000004FCL

//
// %1 flight callsign
// Not Translated
//
// MessageId: AIC_ESTABLISHED
//
// MessageText:
//
//  %1 established%0
//
#define AIC_ESTABLISHED                  0x000004FDL

//
// %1 flight callsign
// Not Translated
//
// MessageId: AIC_BLIND
//
// MessageText:
//
//  %1 blind%0
//
#define AIC_BLIND                        0x000004FEL

//
// %1 flight callsign
// Not Translated
//
// MessageId: AIC_CONTACT
//
// MessageText:
//
//  %1 contact%0
//
#define AIC_CONTACT                      0x000004FFL

//
// %1 flight callsign
// Not Translated
//
// MessageId: AIC_WINGS_LEVEL
//
// MessageText:
//
//  %1 wings level%0
//
#define AIC_WINGS_LEVEL                  0x00000500L

//
// %1 flight callsign
// Not Translated
//
// MessageId: AIC_CHECKING_OUT
//
// MessageText:
//
//  %1 checking out%0
//
#define AIC_CHECKING_OUT                 0x00000501L

//
// %1 flight callsign
// Not Translated
//
// MessageId: AIC_FLIGHT_RTB
//
// MessageText:
//
//  %1 returning to base%0
//
#define AIC_FLIGHT_RTB                   0x00000502L

//
// %1 Tower callsign
//
// %2 Flight callsign
//
// MessageId: AIC_WSO_REQUEST_TAKEOFF_2
//
// MessageText:
//
//  %1, %2 request takeoff clearance%0
//
#define AIC_WSO_REQUEST_TAKEOFF_2        0x00000503L

//
// %1 Tower callsign
//
// %2 Flight callsign
//
// MessageId: AIC_WSO_REQUEST_LANDING_2
//
// MessageText:
//
//  %1, %2 request landing clearance%0
//
#define AIC_WSO_REQUEST_LANDING_2        0x00000504L

//
// %1 flight callsign
// Not Translated
//
// MessageId: AIC_ROLLING
//
// MessageText:
//
//  %1 rolling%0
//
#define AIC_ROLLING                      0x00000505L

//
// %1 Wingman number
//
// MessageId: AIC_ATTACK_MY_TARG_POS_2
//
// MessageText:
//
//  %1, tally your bandit, engaged%0
//
#define AIC_ATTACK_MY_TARG_POS_2         0x00000506L

//
// %1 Wingman number
//
// MessageId: AIC_ATTACK_MY_TARG_NEG_2
//
// MessageText:
//
//  %1, no joy, unable to comply.%0
//
#define AIC_ATTACK_MY_TARG_NEG_2         0x00000507L

//
// %1 Wingman number
//
// MessageId: AIC_HELP_ME_POS_1_A
//
// MessageText:
//
//  %1, visual on lead, on my way.%0
//
#define AIC_HELP_ME_POS_1_A              0x00000508L

//
// %1 Wingman number
//
// MessageId: AIC_HELP_ME_POS_2_A
//
// MessageText:
//
//  %1, visual on lead, tally bandit, 10 seconds.%0
//
#define AIC_HELP_ME_POS_2_A              0x00000509L

//
// %1 Wingman number
//
// MessageId: AIC_HELP_ME_NEG_1_A
//
// MessageText:
//
//  %1, blind, unable to comply.%0
//
#define AIC_HELP_ME_NEG_1_A              0x0000050AL

//
// %1 Wingman number
//
// MessageId: AIC_HELP_ME_NEG_2_A
//
// MessageText:
//
//  %1 is engaged defensive at this time!%0
//
#define AIC_HELP_ME_NEG_2_A              0x0000050BL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_LASER_ON
//
// MessageText:
//
//  %1 Laser on%0
//
#define AIC_WING_LASER_ON                0x0000050CL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_LASER_OFF
//
// MessageText:
//
//  %1 Laser off%0
//
#define AIC_WING_LASER_OFF               0x0000050DL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_DEADEYE
//
// MessageText:
//
//  %1 Deadeye%0
//
#define AIC_WING_DEADEYE                 0x0000050EL

//
// %1 Wingman number
//
// MessageId: AIC_SKOSH
//
// MessageText:
//
//  %1 Skosh%0
//
#define AIC_SKOSH                        0x0000050FL

//
// %1 number of weapon
//
// MessageId: AIC_HARM_COUNT
//
// MessageText:
//
//  %1 harm%0
//
#define AIC_HARM_COUNT                   0x00000510L

//
// %1 number of weapon
//
// MessageId: AIC_A2M_COUNT
//
// MessageText:
//
//  %1 air to mud%0
//
#define AIC_A2M_COUNT                    0x00000511L

//
// %1 number of weapon
//
// MessageId: AIC_GUIDED_COUNT
//
// MessageText:
//
//  %1 guided%0
//
#define AIC_GUIDED_COUNT                 0x00000512L

//
// %1 number of weapon
//
// MessageId: AIC_DUMB_COUNT
//
// MessageText:
//
//  %1 dumb%0
//
#define AIC_DUMB_COUNT                   0x00000513L

//
// %1 number of weapon
//
// MessageId: AIC_CLUSTER_COUNT
//
// MessageText:
//
//  %1 cluster%0
//
#define AIC_CLUSTER_COUNT                0x00000514L

//
// %1 number of weapon
//
// MessageId: AIC_POD_COUNT
//
// MessageText:
//
//  %1 pods%0
//
#define AIC_POD_COUNT                    0x00000515L

//
// %1 Wingman number
//
// MessageId: AIC_WING_BINGO
//
// MessageText:
//
//  %1 bingo%0
//
#define AIC_WING_BINGO                   0x00000516L

//
// %1 Wingman callsign or modex
//
// MessageId: AIC_AIRBORNE
//
// MessageText:
//
//  %1 Airborne%0
//
#define AIC_AIRBORNE                     0x00000517L

//
// %1 Wingman callsign or modex
//
// MessageId: AIC_PASSING_2_5
//
// MessageText:
//
//  %1 passing two point five%0
//
#define AIC_PASSING_2_5                  0x00000518L

//
// %1 Wingman callsign or modex
//
// MessageId: AIC_ARCING
//
// MessageText:
//
//  %1 Arcing%0
//
#define AIC_ARCING                       0x00000519L

//
// %1 Wingman callsign or modex
//
// MessageId: AIC_OUTBOUND
//
// MessageText:
//
//  %1 Outbound%0
//
#define AIC_OUTBOUND                     0x0000051AL

//
// %1 Wingman callsign or modex
//
// MessageId: AIC_UP_FOR_CHECKS
//
// MessageText:
//
//  %1 up for checks%0
//
#define AIC_UP_FOR_CHECKS                0x0000051BL

//
// MessageId: AIC_2_COPY_ACK
//
// MessageText:
//
//  Two copy%0
//
#define AIC_2_COPY_ACK                   0x0000051CL

//
// MessageId: AIC_2_RETURN_REFORM
//
// MessageText:
//
//  Two returning to formation%0
//
#define AIC_2_RETURN_REFORM              0x0000051DL

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_READ_ME_1
//
// MessageText:
//
//  %1,  do you read me?%0
//
#define AIC_SHOT_DOWN_READ_ME_1          0x0000051EL

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_ACK_1
//
// MessageText:
//
//  %1, acknowledged%0
//
#define AIC_SHOT_DOWN_ACK_1              0x0000051FL

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_HIDING_US_1
//
// MessageText:
//
//  %1, enemy near! Hiding!%0
//
#define AIC_SHOT_DOWN_HIDING_US_1        0x00000520L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_UNDERSTOOD_1
//
// MessageText:
//
//  %1, understood%0
//
#define AIC_SHOT_DOWN_UNDERSTOOD_1       0x00000521L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_SAFE_1
//
// MessageText:
//
//  %1, I am down safely!%0
//
#define AIC_SHOT_DOWN_SAFE_1             0x00000522L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_BEAT_UP_US_2
//
// MessageText:
//
//  %1, I'm pretty beat up, send help!%0
//
#define AIC_SHOT_DOWN_BEAT_UP_US_2       0x00000523L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_SAR_SEEN
//
// MessageText:
//
//  %1, SAR helo in sight, marking!%0
//
#define AIC_SHOT_DOWN_SAR_SEEN           0x00000524L

//
// %1 callsign
//
// MessageId: AIC_BDA_SURE
//
// MessageText:
//
//  %1, we can sure confirm the BDA on that target!%0
//
#define AIC_BDA_SURE                     0x00000525L

//
// %1 callsign
//
// MessageId: AIC_TARGET_SMOKING
//
// MessageText:
//
//  %1 lots of smoke still coming from that target.%0
//
#define AIC_TARGET_SMOKING               0x00000526L

//
// %1 Tower callsign
//
// %2 flight callsign
//
// %3 number of aircraft
//
// %4 airplane type (Super Hornet(s))
// Not Translated
//
// MessageId: AIC_TAXI
//
// MessageText:
//
//  %1, %2, taxi, %3 %4%0
//
#define AIC_TAXI                         0x00000527L

// Not Translated
//
// MessageId: AIC_SUPER_HORNETS
//
// MessageText:
//
//  Super Hornets%0
//
#define AIC_SUPER_HORNETS                0x00000528L

// Not Translated
//
// MessageId: AIC_SUPER_HORNET
//
// MessageText:
//
//  Super Hornet%0
//
#define AIC_SUPER_HORNET                 0x00000529L

// Not Translated
//
// MessageId: AIC_HAWKEYES
//
// MessageText:
//
//  Hawkeyes%0
//
#define AIC_HAWKEYES                     0x0000052AL

// Not Translated
//
// MessageId: AIC_HAWKEYE
//
// MessageText:
//
//  Hawkeye%0
//
#define AIC_HAWKEYE                      0x0000052BL

// Not Translated
//
// MessageId: AIC_SENTRIES
//
// MessageText:
//
//  Sentries%0
//
#define AIC_SENTRIES                     0x0000052CL

// Not Translated
//
// MessageId: AIC_SENTRY
//
// MessageText:
//
//  Sentry%0
//
#define AIC_SENTRY                       0x0000052DL

//
// %1 callsign
//
// MessageId: AIC_HOLDING_SHORT
//
// MessageText:
//
//  %1 holding short%0
//
#define AIC_HOLDING_SHORT                0x0000052EL

//
// %1 callsign
//
// MessageId: AIC_ON_TO_HOLD
//
// MessageText:
//
//  %1 on to hold%0
//
#define AIC_ON_TO_HOLD                   0x0000052FL

//
// %1 callsign
//
// MessageId: AIC_ROGER_TAKEOFF
//
// MessageText:
//
//  %1 roger, cleared for takeoff%0
//
#define AIC_ROGER_TAKEOFF                0x00000530L

//
// %1 Departure callsign
//
// %2 player callsign
//
// %3 altitude2
// Not Translated
//
// MessageId: AIC_PASSING_ALT
//
// MessageText:
//
//  %1, %2 passing %3%0
//
#define AIC_PASSING_ALT                  0x00000531L

//
// %1 Arrival callsign
//
// %2 player callsign
//
// %3 range
//
// %4 cardinal
// Not Translated
//
// MessageId: AIC_ARRIVAL_FIELD
//
// MessageText:
//
//  %1, %2, %3 %4 of field%0
//
#define AIC_ARRIVAL_FIELD                0x00000532L

//
// %1 callsign
//
// MessageId: AIC_REQUEST_STRAIGHT
//
// MessageText:
//
//  %1 request straight in%0
//
#define AIC_REQUEST_STRAIGHT             0x00000533L

//
// %1 callsign
//
// MessageId: AIC_EMERGENCY_STRAIGHT
//
// MessageText:
//
//  %1 declaring an emergency, request straight in.%0
//
#define AIC_EMERGENCY_STRAIGHT           0x00000534L

//
// %1 Player callsign
//
// %2 altitude2
//
// %3 Arrival ack 2 (see below)
// Not Translated
//
// MessageId: AIC_DECEND_TO
//
// MessageText:
//
//  %1 decend to %2, %3%0
//
#define AIC_DECEND_TO                    0x00000535L

//
// %1 Player callsign
//
// %2 altitude2
//
// %3 Arrival ack 2 (see below)
// Not Translated
//
// MessageId: AIC_CLIMB_TO
//
// MessageText:
//
//  %1, climb to %2, %3%0
//
#define AIC_CLIMB_TO                     0x00000536L

//
// %1 Player callsign
//
// %2 altitude2
//
// %3 Arrival ack 2 (see below)
// Not Translated
//
// MessageId: AIC_MAINTAIN
//
// MessageText:
//
//  %1, maintain %2, %3%0
//
#define AIC_MAINTAIN                     0x00000537L

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_ARRIVAL_ACK_2
//
// MessageText:
//
//  heading %1%0
//
#define AIC_ARRIVAL_ACK_2                0x00000538L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_RUNWAY_IN_SIGHT
//
// MessageText:
//
//  %1 runway in sight%0
//
#define AIC_RUNWAY_IN_SIGHT              0x00000539L

//
// %1 Tower callsign
//
// %2 player callsign
// Not Translated
//
// MessageId: AIC_TOWER_CHECK_1
//
// MessageText:
//
//  %1, %2, field in sight for straight in.%0
//
#define AIC_TOWER_CHECK_1                0x0000053AL

//
// %1 player callsign
// Not Translated
//
// MessageId: AIC_TOWER_CHECK_2
//
// MessageText:
//
//  %1, five miles, gear down full stop%0
//
#define AIC_TOWER_CHECK_2                0x0000053BL

//
// %1 player callsign
// Not Translated
//
// MessageId: AIC_TOWER_CHECK_3
//
// MessageText:
//
//  %1, five miles%0
//
#define AIC_TOWER_CHECK_3                0x0000053CL

// Not Translated
//
// MessageId: AIC_GECKO
//
// MessageText:
//
//  Gecko%0
//
#define AIC_GECKO                        0x0000053DL

// Not Translated
//
// MessageId: AIC_GRUMBLE
//
// MessageText:
//
//  Grumble%0
//
#define AIC_GRUMBLE                      0x0000053EL

// Not Translated
//
// MessageId: AIC_GADFLY
//
// MessageText:
//
//  Gadfly%0
//
#define AIC_GADFLY                       0x0000053FL

// Not Translated
//
// MessageId: AIC_GAUNTLET
//
// MessageText:
//
//  Gauntlet%0
//
#define AIC_GAUNTLET                     0x00000540L

// Not Translated
//
// MessageId: AIC_TRIPLE_A
//
// MessageText:
//
//  Triple A%0
//
#define AIC_TRIPLE_A                     0x00000541L

// Not Translated
//
// MessageId: AIC_EW
//
// MessageText:
//
//  EW%0
//
#define AIC_EW                           0x00000542L

//
// %1 callsign
// Not Translated
//
// MessageId: AIC_AFFIRMATIVE
//
// MessageText:
//
//  %1 affirmative%0
//
#define AIC_AFFIRMATIVE                  0x00000543L

//
// %1 callsign
// Not Translated
//
// MessageId: AIC_STANDBY
//
// MessageText:
//
//  %1 standby%0
//
#define AIC_STANDBY                      0x00000544L

//
// %1 callsign
// Not Translated
//
// MessageId: AIC_STANDBY_FOR_WORDS
//
// MessageText:
//
//  %1 copy, standby for words%0
//
#define AIC_STANDBY_FOR_WORDS            0x00000545L

//
// %1 callsign
// Not Translated
//
// MessageId: AIC_STANDBY_FOR_NINE
//
// MessageText:
//
//  %1 copy, standby for nine line brief%0
//
#define AIC_STANDBY_FOR_NINE             0x00000546L

//
// %1 callsign
// Not Translated
//
// MessageId: AIC_STANDBY_FOR_FOUR
//
// MessageText:
//
//  %1 copy, standby for four line brief%0
//
#define AIC_STANDBY_FOR_FOUR             0x00000547L

//
// %1 callsign
//
// %2 IP info
// Not Translated
//
// MessageId: AIC_NINE_LINE_1
//
// MessageText:
//
//  %1, nine line as follows, I P %2%0
//
#define AIC_NINE_LINE_1                  0x00000548L

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_NINE_LINE_2
//
// MessageText:
//
//  target bearing %1,%0
//
#define AIC_NINE_LINE_2                  0x00000549L

//
// %1 range
// Not Translated
//
// MessageId: AIC_NINE_LINE_3
//
// MessageText:
//
//  distance %1,%0
//
#define AIC_NINE_LINE_3                  0x0000054AL

//
// %1 elevation
// Not Translated
//
// MessageId: AIC_NINE_LINE_4
//
// MessageText:
//
//  Target elevation %1,%0
//
#define AIC_NINE_LINE_4                  0x0000054BL

//
// %1 target type
// Not Translated
//
// MessageId: AIC_NINE_LINE_5_6
//
// MessageText:
//
//  %1, sending coordinates on dolly%0
//
#define AIC_NINE_LINE_5_6                0x0000054CL

// Not Translated
//
// MessageId: AIC_NINE_LINE_7A
//
// MessageText:
//
//  no mark point%0
//
#define AIC_NINE_LINE_7A                 0x0000054DL

//
// %1 bearing
//
// %2 range
// Not Translated
//
// MessageId: AIC_NINE_LINE_7B
//
// MessageText:
//
//  smoke marker %1, %2,%0
//
#define AIC_NINE_LINE_7B                 0x0000054EL

// Not Translated
//
// MessageId: AIC_NINE_LINE_7C
//
// MessageText:
//
//  smoke marker on target%0
//
#define AIC_NINE_LINE_7C                 0x0000054FL

// Not Translated
//
// MessageId: AIC_NINE_LINE_8A
//
// MessageText:
//
//  No friendlies in area%0
//
#define AIC_NINE_LINE_8A                 0x00000550L

//
// %1 bearing
//
// %2 range
// Not Translated
//
// MessageId: AIC_NINE_LINE_8B
//
// MessageText:
//
//  Friendlies bearing %1, %2%0
//
#define AIC_NINE_LINE_8B                 0x00000551L

// Not Translated
//
// MessageId: AIC_NINE_LINE_9A
//
// MessageText:
//
//  No threats in area%0
//
#define AIC_NINE_LINE_9A                 0x00000552L

//
// %1 threat type
//
// %2 bearing
//
// %3 range
// Not Translated
//
// MessageId: AIC_NINE_LINE_9B
//
// MessageText:
//
//  Threats %1, %2 %3,%0
//
#define AIC_NINE_LINE_9B                 0x00000553L

// Not Translated
//
// MessageId: AIC_FOUR_LINE_FOLLOWS
//
// MessageText:
//
//  Four line as follows%0
//
#define AIC_FOUR_LINE_FOLLOWS            0x00000554L

//
// %1 weather
// Not Translated
//
// MessageId: AIC_FOUR_LINE_WEATHER
//
// MessageText:
//
//  weather is %1%0
//
#define AIC_FOUR_LINE_WEATHER            0x00000555L

//
// %1 FAC callsign
// Not Translated
//
// MessageId: AIC_FAC_RELEASE_SMOKE
//
// MessageText:
//
//  %1 releasing smoke mark%0
//
#define AIC_FAC_RELEASE_SMOKE            0x00000556L

//
// %1 FAC callsign
// Not Translated
//
// MessageId: AIC_FAC_LASER_ON
//
// MessageText:
//
//  %1 laser on, sparkle%0
//
#define AIC_FAC_LASER_ON                 0x00000557L

//
// %1 FAC callsign
// Not Translated
//
// MessageId: AIC_FAC_ILLUMINATED
//
// MessageText:
//
//  %1 target illuminated%0
//
#define AIC_FAC_ILLUMINATED              0x00000558L

//
// %1 FAC callsign
// Not Translated
//
// MessageId: AIC_FAC_SMOKE_PLACED
//
// MessageText:
//
//  %1 smoke placed%0
//
#define AIC_FAC_SMOKE_PLACED             0x00000559L

// Not Translated
//
// MessageId: AIC_HIT
//
// MessageText:
//
//  Hit%0
//
#define AIC_HIT                          0x0000055AL

//
// %1 distance
// Not Translated
//
// MessageId: AIC_MISS_SHORT
//
// MessageText:
//
//  Miss, short %1%0
//
#define AIC_MISS_SHORT                   0x0000055BL

//
// %1 distance
// Not Translated
//
// MessageId: AIC_MISS_LONG
//
// MessageText:
//
//  Miss, long %1%0
//
#define AIC_MISS_LONG                    0x0000055CL

//
// %1 distance
// Not Translated
//
// MessageId: AIC_MISS_LEFT
//
// MessageText:
//
//  Miss, left %1%0
//
#define AIC_MISS_LEFT                    0x0000055DL

//
// %1 distance
// Not Translated
//
// MessageId: AIC_MISS_RIGHT
//
// MessageText:
//
//  Miss, right %1%0
//
#define AIC_MISS_RIGHT                   0x0000055EL

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_FRIENDLY_FIRE_US_4
//
// MessageText:
//
//  %1, Check fire!  Check fire!  Your weapons are landing within friendly lines!%0
//
#define AIC_GROUND_FRIENDLY_FIRE_US_4    0x0000055FL

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_FRIENDLY_FIRE_US_5
//
// MessageText:
//
//  %1 Abort your attack!  Friendlies are in that area!%0
//
#define AIC_GROUND_FRIENDLY_FIRE_US_5    0x00000560L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_FRIENDLY_FIRE_US_6
//
// MessageText:
//
//  %1 Cease fire!  Bittersweet, repeat Bittersweet!%0
//
#define AIC_GROUND_FRIENDLY_FIRE_US_6    0x00000561L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_GOOD_US_4
//
// MessageText:
//
//  %1 You got all of them!  Great job!%0
//
#define AIC_GROUND_GOOD_US_4             0x00000562L

//
// %1 Player callsign
//
// MessageId: AIC_GROUND_GOOD_US_5
//
// MessageText:
//
//  %1 Good work!  Remaining enemy forces are surrendering!%0
//
#define AIC_GROUND_GOOD_US_5             0x00000563L

//
// %1 Player callsign
//
// MessageId: AIC_FAC_TARGET_CLEAR
//
// MessageText:
//
//  %1 be advised, target area appears clear%0
//
#define AIC_FAC_TARGET_CLEAR             0x00000564L

//
// %1 Player callsign
//
// MessageId: AIC_COPY_RTB
//
// MessageText:
//
//  %1 copy RTB%0
//
#define AIC_COPY_RTB                     0x00000565L

//
// %1 thousands digit
//
// %2 hundreds digit
//
// %3 tens digit
//
// %4 ones digit
// Not Translated
//
// MessageId: AIC_DIST_FEET
//
// MessageText:
//
//  %1 feet%0
//
#define AIC_DIST_FEET                    0x00000566L

//
// %1 numeric digit
// Not Translated
//
// MessageId: AIC_IP
//
// MessageText:
//
//  Waypoint %1%0
//
#define AIC_IP                           0x00000567L

// Not Translated
//
// MessageId: AIC_WEATHER_1
//
// MessageText:
//
//  clear, visability unlimited%0
//
#define AIC_WEATHER_1                    0x00000568L

//
// %1 range
// Not Translated
//
// MessageId: AIC_WEATHER_2
//
// MessageText:
//
//  scattered, visability %1%0
//
#define AIC_WEATHER_2                    0x00000569L

//
// %1 range
// Not Translated
//
// MessageId: AIC_WEATHER_3
//
// MessageText:
//
//  broken, visability %1%0
//
#define AIC_WEATHER_3                    0x0000056AL

//
// %1 range
// Not Translated
//
// MessageId: AIC_WEATHER_4
//
// MessageText:
//
//  overcast, visability%0
//
#define AIC_WEATHER_4                    0x0000056BL

// Not Translated
//
// MessageId: AIC_TANKS
//
// MessageText:
//
//  tanks %0
//
#define AIC_TANKS                        0x0000056CL

// Not Translated
//
// MessageId: AIC_APCS
//
// MessageText:
//
//  A P C's%0
//
#define AIC_APCS                         0x0000056DL

// Not Translated
//
// MessageId: AIC_ARMOR
//
// MessageText:
//
//  armor%0
//
#define AIC_ARMOR                        0x0000056EL

// Not Translated
//
// MessageId: AIC_TRUCKS
//
// MessageText:
//
//  trucks%0
//
#define AIC_TRUCKS                       0x0000056FL

// Not Translated
//
// MessageId: AIC_ARTILLERY
//
// MessageText:
//
//  artillery site%0
//
#define AIC_ARTILLERY                    0x00000570L

// Not Translated
//
// MessageId: AIC_AAA_SITE
//
// MessageText:
//
//  triple A site%0
//
#define AIC_AAA_SITE                     0x00000571L

// Not Translated
//
// MessageId: AIC_MOBILE_AAA_SITE
//
// MessageText:
//
//  moble triple A%0
//
#define AIC_MOBILE_AAA_SITE              0x00000572L

// Not Translated
//
// MessageId: AIC_SAM_SITE
//
// MessageText:
//
//  SAM site%0
//
#define AIC_SAM_SITE                     0x00000573L

// Not Translated
//
// MessageId: AIC_MOBILE_SAM_SITE
//
// MessageText:
//
//  moble SAM%0
//
#define AIC_MOBILE_SAM_SITE              0x00000574L

// Not Translated
//
// MessageId: AIC_TROOPS
//
// MessageText:
//
//  troops%0
//
#define AIC_TROOPS                       0x00000575L

// Not Translated
//
// MessageId: AIC_SUPPLY_DUMP
//
// MessageText:
//
//  supply dump%0
//
#define AIC_SUPPLY_DUMP                  0x00000576L

// Not Translated
//
// MessageId: AIC_AAA_BEARING
//
// MessageText:
//
//  Triple A bearing%0
//
#define AIC_AAA_BEARING                  0x00000577L

// Not Translated
//
// MessageId: AIC_MANPADS_BEARING
//
// MessageText:
//
//  Manpads bearing%0
//
#define AIC_MANPADS_BEARING              0x00000578L

// Not Translated
//
// MessageId: AIC_MOBLE_IR_BEARING
//
// MessageText:
//
//  Mobile IR SAM bearing%0
//
#define AIC_MOBLE_IR_BEARING             0x00000579L

// Not Translated
//
// MessageId: AIC_MOBILE_RADAR_BEARING
//
// MessageText:
//
//  Mobile radar SAM bearing%0
//
#define AIC_MOBILE_RADAR_BEARING         0x0000057AL

// Not Translated
//
// MessageId: AIC_RADAR_SAM_BEARING
//
// MessageText:
//
//  radar SAM site bearing%0
//
#define AIC_RADAR_SAM_BEARING            0x0000057BL

//
// %1Flight callsign
//
// %2 Clock position
// Not Translated
//
// MessageId: AIC_WING_HEADS_UP_MISSILE_RAF
//
// MessageText:
//
//  %1 Watch it lads!  Missile Launch, %2!%0
//
#define AIC_WING_HEADS_UP_MISSILE_RAF    0x0000057CL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_TARGETS_SIGHTED_IN_HOT_RAF
//
// MessageText:
//
//  %1, targets in sight, here we go then%0
//
#define AIC_WING_TARGETS_SIGHTED_IN_HOT_RAF 0x0000057DL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_EGAGING_GOMERS_RAF
//
// MessageText:
//
//  %1 enemy troops spotted, attacking%0
//
#define AIC_WING_EGAGING_GOMERS_RAF      0x0000057EL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_DUG_GUNS_RAF
//
// MessageText:
//
//  %1 dug in weapons spotted, roving in.%0
//
#define AIC_WING_ATTACKING_DUG_GUNS_RAF  0x0000057FL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_TRUCKS_RAF
//
// MessageText:
//
//  %1 lories in sight, commencing attack.%0
//
#define AIC_WING_ATTACKING_TRUCKS_RAF    0x00000580L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_AAA_RAF
//
// MessageText:
//
//  %1 Flak site spotted, attacking.%0
//
#define AIC_WING_ATTACKING_AAA_RAF       0x00000581L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_MOBILE_SAM_RAF
//
// MessageText:
//
//  %1 mobile SAM site spotted, roving in%0
//
#define AIC_WING_ATTACKING_MOBILE_SAM_RAF 0x00000582L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_SHIP_RAF
//
// MessageText:
//
//  %1 enemy warship in sight, attacking the bugger now.%0
//
#define AIC_WING_ATTACKING_SHIP_RAF      0x00000583L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_CAN_RAF
//
// MessageText:
//
//  %1, enemy ship spotted, going after him.%0
//
#define AIC_WING_ATTACKING_CAN_RAF       0x00000584L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_HITS_IN_AREA_RAF
//
// MessageText:
//
//  %1 it appears some of the buggers are unharmed.%0
//
#define AIC_WING_HITS_IN_AREA_RAF        0x00000585L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_HITS_FROM_GROUND_RAF
//
// MessageText:
//
//  %1 the bloody Flak has managed to hit us!%0
//
#define AIC_WING_HITS_FROM_GROUND_RAF    0x00000586L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_WE_HIT_HURT_RAF
//
// MessageText:
//
//  %1 we are wounded, assessing damage now!%0
//
#define AIC_WING_WE_HIT_HURT_RAF         0x00000587L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_EJECTING_RAF_ATK
//
// MessageText:
//
//  %1 bailing out!%0
//
#define AIC_WING_EJECTING_RAF_ATK        0x00000588L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_EJECTING_RAF_FTR
//
// MessageText:
//
//  %1 ejecting!%0
//
#define AIC_WING_EJECTING_RAF_FTR        0x00000589L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_TAKING_AAA_RAF
//
// MessageText:
//
//  %1 I am encountering heavy flak!%0
//
#define AIC_WING_TAKING_AAA_RAF          0x0000058AL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_I_AM_HIT_RAF
//
// MessageText:
//
//  %1, this enemy bugger has managed to hit me!%0
//
#define AIC_WING_I_AM_HIT_RAF            0x0000058BL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_THINK_MISSILE_HIT_RAF
//
// MessageText:
//
//  %1 has just been hit by a missile!%0
//
#define AIC_WING_THINK_MISSILE_HIT_RAF   0x0000058CL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_L_HIT_RAF
//
// MessageText:
//
//  %1, I have been damaged!%0
//
#define AIC_WING_L_HIT_RAF               0x0000058DL

//
// %1 player/wingman callsign
// Not Translated
//
// MessageId: AIC_WING_EJECT_CALL_1_RAF
//
// MessageText:
//
//  %1 you are on fire!  Eject lad!%0
//
#define AIC_WING_EJECT_CALL_1_RAF        0x0000058EL

//
// %1 player/wingman callsign
// Not Translated
//
// MessageId: AIC_WING_EJECT_CALL_3_RAF
//
// MessageText:
//
//  %1 get out of there!  Bail out!%0
//
#define AIC_WING_EJECT_CALL_3_RAF        0x0000058FL

//
// %1 tanker callsign
//
// %2 flight callsign
//
// %3 tanker gives
//
// MessageId: AIC_TANKER_LOOKING_FOR_RAF
//
// MessageText:
//
//  %1, %2, my switches are safe, my nose is cold, I am looking for %3.%0
//
#define AIC_TANKER_LOOKING_FOR_RAF       0x00000590L

//
// %1 Pilot callsign
// Not Translated
//
// MessageId: AIC_SHOT_DOWN_HIDING_US_1_RAF
//
// MessageText:
//
//  %1 enemy chaps approaching my position!%0
//
#define AIC_SHOT_DOWN_HIDING_US_1_RAF    0x00000591L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_SAR_SEEN_ATTACK
//
// MessageText:
//
//  %1 Pilot callsign%0
//
#define AIC_SHOT_DOWN_SAR_SEEN_ATTACK    0x00000592L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_SAR_SEEN_RAF
//
// MessageText:
//
//  %1 SAR helicopter in sight, marking my location!%0
//
#define AIC_SHOT_DOWN_SAR_SEEN_RAF       0x00000593L

//
// %1Flight callsign
//
// %2 Clock position
// Not Translated
//
// MessageId: AIC_WING_HEADS_UP_MISSILE_RUSA
//
// MessageText:
//
//  %1, Comrades!  Missile Launch!%0
//
#define AIC_WING_HEADS_UP_MISSILE_RUSA   0x00000594L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_MUSIC_ON_RUSA
//
// MessageText:
//
//  %1 Jammer On%0
//
#define AIC_WING_MUSIC_ON_RUSA           0x00000595L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_MUSIC_OFF_RUSA
//
// MessageText:
//
//  %1 Jammer Off%0
//
#define AIC_WING_MUSIC_OFF_RUSA          0x00000596L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_TARGETS_SIGHTED_IN_HOT_RUSA
//
// MessageText:
//
//  %1, targets in sight, commencing attack%0
//
#define AIC_WING_TARGETS_SIGHTED_IN_HOT_RUSA 0x00000597L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_DUG_GUNS_RUSA
//
// MessageText:
//
//  %1, artillery spotted, targeting weapons.%0
//
#define AIC_WING_ATTACKING_DUG_GUNS_RUSA 0x00000598L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_AAA_RUSA
//
// MessageText:
//
//  %1 attacking Triple A site.%0
//
#define AIC_WING_ATTACKING_AAA_RUSA      0x00000599L

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_MOBILE_SAM_RUSA
//
// MessageText:
//
//  %1 attacking mobile SAM site%0
//
#define AIC_WING_ATTACKING_MOBILE_SAM_RUSA 0x0000059AL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_ATTACKING_SHIP_RUSA
//
// MessageText:
//
//  %1, enemy warship in sight, attacking now.%0
//
#define AIC_WING_ATTACKING_SHIP_RUSA     0x0000059BL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_WING_HITS_IN_AREA_RUSA
//
// MessageText:
//
//  %1, ground targets not yet destroyed.%0
//
#define AIC_WING_HITS_IN_AREA_RUSA       0x0000059CL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_HITS_FROM_GROUND_RUSA
//
// MessageText:
//
//  %1 I have been damaged%0
//
#define AIC_WING_HITS_FROM_GROUND_RUSA   0x0000059DL

//
// %1 Wingman callsign
//
// MessageId: AIC_WING_WE_HIT_RUSA
//
// MessageText:
//
//  %1 we have been hit!%0
//
#define AIC_WING_WE_HIT_RUSA             0x0000059EL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_WE_HIT_HURT_RUSA
//
// MessageText:
//
//  %1, we are wounded.%0
//
#define AIC_WING_WE_HIT_HURT_RUSA        0x0000059FL

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_TAKING_AAA_RUSA
//
// MessageText:
//
//  %1 I am encountering heavy ground fire!!%0
//
#define AIC_WING_TAKING_AAA_RUSA         0x000005A0L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_I_AM_HIT_RUSA
//
// MessageText:
//
//  %1, this enemy has managed to hit me!%0
//
#define AIC_WING_I_AM_HIT_RUSA           0x000005A1L

//
// %1 player/wingman callsign
// Not Translated
//
// MessageId: AIC_WING_EJECT_CALL_1_RUSA
//
// MessageText:
//
//  %1 you are on fire!  Eject Comrade!%0
//
#define AIC_WING_EJECT_CALL_1_RUSA       0x000005A2L

//
// %1 Flight callsign
// Not Translated
//
// MessageId: AIC_BEGINNING_SWEEP_RAF
//
// MessageText:
//
//  %1 roving into the target area now.%0
//
#define AIC_BEGINNING_SWEEP_RAF          0x000005A3L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_HIDING_RUSA_1
//
// MessageText:
//
//  %1, enemy near!%0
//
#define AIC_SHOT_DOWN_HIDING_RUSA_1      0x000005A4L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_BEAT_UP_RUSA_2
//
// MessageText:
//
//  %1, I am injured!%0
//
#define AIC_SHOT_DOWN_BEAT_UP_RUSA_2     0x000005A5L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_SAR_SEEN_RUSA
//
// MessageText:
//
//  %1, Rescue helicopter in sight!%0
//
#define AIC_SHOT_DOWN_SAR_SEEN_RUSA      0x000005A6L

//
// %1 Pilot callsign
//
// %2 runway
//
// MessageId: AIC_TAXI_TO_RUNWAY
//
// MessageText:
//
//  %1, taxi to runway %2%0
//
#define AIC_TAXI_TO_RUNWAY               0x000005A7L

//
// %1 Pilot callsign
//
// MessageId: AIC_CLEARED_FOR_TAKEOFF_DEPARTURE
//
// MessageText:
//
//  %1 cleared for takeoff, contact Departure%0
//
#define AIC_CLEARED_FOR_TAKEOFF_DEPARTURE 0x000005A8L

//
// %1 Player callsign
//
// %2 winds string
//
// %3 ceiling string
//
// %4 visablitly
//
// MessageId: AIC_TOWER_CLEAR_TAKEOFF_2
//
// MessageText:
//
//  %1, %2, %3 %4 cleared for takeoff%0
//
#define AIC_TOWER_CLEAR_TAKEOFF_2        0x000005A9L

//
// %1 Pilot callsign
//
// MessageId: AIC_TOWER_HOLD_SHORT_1
//
// MessageText:
//
//  %1 hold short for arriving traffic%0
//
#define AIC_TOWER_HOLD_SHORT_1           0x000005AAL

//
// %1 Pilot callsign
//
// MessageId: AIC_TOWER_AWAIT_IFR
//
// MessageText:
//
//  %1, Taxi into position and hold, awaiting IFR release.%0
//
#define AIC_TOWER_AWAIT_IFR              0x000005ABL

//
// %1 Player callsign
//
// %2 bearing
//
// %3 departure 2
// Not Translated
//
// MessageId: AIC_TOWER_DEPARTURE_CONTINUE
//
// MessageText:
//
//  %1, Departure radar contact, continue on course %2, %3.%0
//
#define AIC_TOWER_DEPARTURE_CONTINUE     0x000005ACL

//
// %1 Player callsign
//
// %2 bearing
//
// %3 departure 2
// Not Translated
//
// MessageId: AIC_TOWER_DEPARTURE_LEFT
//
// MessageText:
//
//  %1, Departure radar contact, turn left to %2, %3%0
//
#define AIC_TOWER_DEPARTURE_LEFT         0x000005ADL

//
// %1 Player callsign
//
// %2 bearing
//
// %3 departure 2
// Not Translated
//
// MessageId: AIC_TOWER_DEPARTURE_RIGHT
//
// MessageText:
//
//  %1, Departure radar contact, turn right to %2, %3%0
//
#define AIC_TOWER_DEPARTURE_RIGHT        0x000005AEL

//
// %1 altitude2
// Not Translated
//
// MessageId: AIC_DEPART2_CLIMB
//
// MessageText:
//
//  %1 climb and maintain %2%0
//
#define AIC_DEPART2_CLIMB                0x000005AFL

//
// %1 altitude2
// Not Translated
//
// MessageId: AIC_DEPART2_DESCEND
//
// MessageText:
//
//  %1 descend and maintain%0
//
#define AIC_DEPART2_DESCEND              0x000005B0L

//
// %1 altitude2
// Not Translated
//
// MessageId: AIC_DEPART2_MAINTAIN
//
// MessageText:
//
//  %1 maintain%0
//
#define AIC_DEPART2_MAINTAIN             0x000005B1L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_ARRIVAL_CHECK_IN
//
// MessageText:
//
//  %1, Arrival, radar contact, state intentions.%0
//
#define AIC_ARRIVAL_CHECK_IN             0x000005B2L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_ROGER_CALLSIGN
//
// MessageText:
//
//  Roger %1%0
//
#define AIC_ROGER_CALLSIGN               0x000005B3L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_ARRIVAL_EMERGENCY
//
// MessageText:
//
//  Understand you are declaring an emergency %1%0
//
#define AIC_ARRIVAL_EMERGENCY            0x000005B4L

//
// %1 Player callsign
//
// %2 AWACS callsign
// Not Translated
//
// MessageId: AIC_FIELD_CLOSED_1
//
// MessageText:
//
//  %1 the field is closed, contact %2 for new divert.%0
//
#define AIC_FIELD_CLOSED_1               0x000005B5L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_FIELD_CLOSED_2
//
// MessageText:
//
//  %1 the field is closed.%0
//
#define AIC_FIELD_CLOSED_2               0x000005B6L

//
// %1 Player callsign
//
// %2 altitude2
//
// %3 arrival control part 2
// Not Translated
//
// MessageId: AIC_DESCEND_TO_ARRIVAL
//
// MessageText:
//
//  %1, descend to %2, %3%0
//
#define AIC_DESCEND_TO_ARRIVAL           0x000005B7L

//
// %1 Player callsign
//
// %2 altitude2
//
// %3 arrival control part 2
// Not Translated
//
// MessageId: AIC_CLIMB_TO_ARRIVAL
//
// MessageText:
//
//  %1, climb to %2, %3%0
//
#define AIC_CLIMB_TO_ARRIVAL             0x000005B8L

//
// %1 Player callsign
//
// %2 altitude2
//
// %3 arrival control part 2
// Not Translated
//
// MessageId: AIC_MAINTAIN_ARRIVAL
//
// MessageText:
//
//  %1, maintain %2, %3%0
//
#define AIC_MAINTAIN_ARRIVAL             0x000005B9L

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_TURN_RIGHT_TO
//
// MessageText:
//
//  turn right to %1%0
//
#define AIC_TURN_RIGHT_TO                0x000005BAL

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_TURN_LEFT_TO
//
// MessageText:
//
//  turn left to %1%0
//
#define AIC_TURN_LEFT_TO                 0x000005BBL

//
// %1 bearing
// Not Translated
//
// MessageId: AIC_CONTINUE_COURSE_TO
//
// MessageText:
//
//  continue on course %1%0
//
#define AIC_CONTINUE_COURSE_TO           0x000005BCL

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_CALL_RUNWAY
//
// MessageText:
//
//  %1 call runway in sight%0
//
#define AIC_CALL_RUNWAY                  0x000005BDL

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_CONTACT_TOWER
//
// MessageText:
//
//  %1 contact Tower%0
//
#define AIC_CONTACT_TOWER                0x000005BEL

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_REPORT_5_MILE
//
// MessageText:
//
//  %1 report 5 mile final%0
//
#define AIC_REPORT_5_MILE                0x000005BFL

//
// %1 Player callsign
//
// %2 winds string
//
// %3 runway
// Not Translated
//
// MessageId: AIC_LAND_RUNWAY_N
//
// MessageText:
//
//  %1, %2, cleared to land on runway %3%0
//
#define AIC_LAND_RUNWAY_N                0x000005C0L

//
// %1 Player callsign
//
// %2 runway
// Not Translated
//
// MessageId: AIC_CLEARED_RUNWAY_N
//
// MessageText:
//
//  %1 check wheels down, cleared to land on runway %2%0
//
#define AIC_CLEARED_RUNWAY_N             0x000005C1L

//
// %1 Player callsign
//
// MessageId: AIC_TOWER_CLEAR_LAND_1
//
// MessageText:
//
//  %1, cleared for landing.%0
//
#define AIC_TOWER_CLEAR_LAND_1           0x000005C2L

//
// %1 Player callsign
//
// %2 angels
// Not Translated
//
// MessageId: AIC_HOLD_TRAFFIC_ANGELS
//
// MessageText:
//
//  %1 hold for traffic at %2%0
//
#define AIC_HOLD_TRAFFIC_ANGELS          0x000005C3L

//
// %1 Player callsign
//
// %2 bearing
//
// %3 range
//
// %4 altitude
//
// MessageId: AIC_TOWER_FRIENDLY_TRAFFIC_2
//
// MessageText:
//
//  %1 friendly traffic bearing %2, %3, %4%0
//
#define AIC_TOWER_FRIENDLY_TRAFFIC_2     0x000005C4L

//
// %1 Player callsign
//
// %2 cardinal direction
//
// MessageId: AIC_TOWER_SCRAMBLE_1
//
// MessageText:
//
//  %1, scramble, scramble, bogies inbound from the %2.%0
//
#define AIC_TOWER_SCRAMBLE_1             0x000005C5L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_TAXI_TO_PARKING
//
// MessageText:
//
//  %1 taxi to parking%0
//
#define AIC_TAXI_TO_PARKING              0x000005C6L

//
// %1 wind direction
// Not Translated
//
// MessageId: AIC_WINDS
//
// MessageText:
//
//  winds %1%0
//
#define AIC_WINDS                        0x000005C7L

//
// %1 bearing
//
// %2 wind speed
// Not Translated
//
// MessageId: AIC_TOWER_WIND_DIR_2
//
// MessageText:
//
//  %1 at %2%0
//
#define AIC_TOWER_WIND_DIR_2             0x000005C8L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_AIRSPACE_VIOLATION_1
//
// MessageText:
//
//  %1, you are violating our airspace.  Reverse your course at once%0
//
#define AIC_AIRSPACE_VIOLATION_1         0x000005C9L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_AIRSPACE_VIOLATION_2
//
// MessageText:
//
//  %1, be advised that you are apporaching our airspace boundary%0
//
#define AIC_AIRSPACE_VIOLATION_2         0x000005CAL

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_AIRSPACE_VIOLATION_3
//
// MessageText:
//
//  %1, leave our airspace at once or we weill be forced to shoot you down.%0
//
#define AIC_AIRSPACE_VIOLATION_3         0x000005CBL

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_AIRSPACE_VIOLATION_4
//
// MessageText:
//
//  %1, this is your final warning.  Leave our airspace or you will be shot down.%0
//
#define AIC_AIRSPACE_VIOLATION_4         0x000005CCL

//
// %1 AWACS callsign
//
// %2 SAR callsign
// Not Translated
//
// MessageId: AIC_SAR_PICKUP_OK_NAVY
//
// MessageText:
//
//  %1, %2, pickup successful, returning to homeplate.%0
//
#define AIC_SAR_PICKUP_OK_NAVY           0x000005CDL

//
// %1 AWACS callsign
//
// %2 SAR callsign
// Not Translated
//
// MessageId: AIC_SAR_NO_FRIENDLIES_NAVY
//
// MessageText:
//
//  %1, %2, no friendlies at LZ, returning to homeplate.%0
//
#define AIC_SAR_NO_FRIENDLIES_NAVY       0x000005CEL

//
// %1 AWACS callsign
//
// %2 bearing
//
// %3 range
//
// MessageId: AIC_AWACS_NEW_THREAT_BEARING_1
//
// MessageText:
//
//  %1, Be advised, new threats airborne at bearing %2, %3%0
//
#define AIC_AWACS_NEW_THREAT_BEARING_1   0x000005CFL

//
// %1 AWACS callsign
//
// MessageId: AIC_AWACS_MERGED_1
//
// MessageText:
//
//  %1, merged.%0
//
#define AIC_AWACS_MERGED_1               0x000005D0L

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_COPY_1
//
// MessageText:
//
//  %1, copy%0
//
#define AIC_AWACS_COPY_1                 0x000005D1L

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_ACKNOWLEDGED_1
//
// MessageText:
//
//  %1, acknowledged.%0
//
#define AIC_AWACS_ACKNOWLEDGED_1         0x000005D2L

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_SAR_ACK_1
//
// MessageText:
//
//  %1, copy, SAR package enroute.%0
//
#define AIC_AWACS_SAR_ACK_1              0x000005D3L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_AWACS_COPY_CLEARED_PATROL
//
// MessageText:
//
//  %1, copy, patrol as fragged.%0
//
#define AIC_AWACS_COPY_CLEARED_PATROL    0x000005D4L

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_VECTORING_FIGHTERS_1
//
// MessageText:
//
//  %1, copy, vectoring fighters to your location.%0
//
#define AIC_AWACS_VECTORING_FIGHTERS_1   0x000005D5L

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_NO_FIGHTERS_1
//
// MessageText:
//
//  %1, negative, no assets available.%0
//
#define AIC_AWACS_NO_FIGHTERS_1          0x000005D6L

//
// %1 Callsign or modex
// Not Translated
//
// MessageId: AIC_AWACS_HELP_ON_WAY
//
// MessageText:
//
//  %1, standby, help is on the way.%0
//
#define AIC_AWACS_HELP_ON_WAY            0x000005D7L

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_WEASELS_1
//
// MessageText:
//
//  %1, copy, Weasel support inbound.%0
//
#define AIC_AWACS_WEASELS_1              0x000005D8L

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_NO_WEASELS_1
//
// MessageText:
//
//  %1, negative, no Weasels available.%0
//
#define AIC_AWACS_NO_WEASELS_1           0x000005D9L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_AWACS_SCRAMMING
//
// MessageText:
//
//  %1 Scramming%0
//
#define AIC_AWACS_SCRAMMING              0x000005DAL

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_TASKING_REQUEST
//
// MessageText:
//
//  %1 priority tasking, Can you comply?%0
//
#define AIC_AWACS_TASKING_REQUEST        0x000005DBL

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_STANDBY_1
//
// MessageText:
//
//  %1,  We have priority tasking for you.  Standby for words.%0
//
#define AIC_AWACS_STANDBY_1              0x000005DCL

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_SUPPORT_GROUND_1
//
// MessageText:
//
//  %1, Vector immediately to new coordinates to support ground forces.%0
//
#define AIC_AWACS_SUPPORT_GROUND_1       0x000005DDL

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_NEW_COORDINATES_1
//
// MessageText:
//
//  %1, transmitting new coordinates on dolly.%0
//
#define AIC_AWACS_NEW_COORDINATES_1      0x000005DEL

//
// %1 Player callsign
//
// MessageId: AIC_AWACS_GROUND_FAC_1
//
// MessageText:
//
//  %1, upon arrival at new coordinates hold and orbit.  Ground FAC will contact you.%0
//
#define AIC_AWACS_GROUND_FAC_1           0x000005DFL

//
// %1 AWACS callsign
//
// %2 bearing
// Not Translated
//
// MessageId: AIC_AWACS_BURNTHROUGH
//
// MessageText:
//
//  %1 burnthrough %2%0
//
#define AIC_AWACS_BURNTHROUGH            0x000005E0L

//
// %1 AWACS callsign
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_AWACS_SPITTER
//
// MessageText:
//
//  %1 spitter %2%0
//
#define AIC_AWACS_SPITTER                0x000005E1L

//
// %1 AWACS callsign
//
// %2 bullseye or tactical (bra) info
// Not Translated
//
// MessageId: AIC_AWACS_LEAKERS
//
// MessageText:
//
//  %1, leakers %2%0
//
#define AIC_AWACS_LEAKERS                0x000005E2L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_AWACS_BITTERSWEET
//
// MessageText:
//
//  %1, bittersweet%0
//
#define AIC_AWACS_BITTERSWEET            0x000005E3L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_AWACS_ROE_RED
//
// MessageText:
//
//  %1, Warning Red%0
//
#define AIC_AWACS_ROE_RED                0x000005E4L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_AWACS_ROE_YELLOW
//
// MessageText:
//
//  %1, Warning Yellow%0
//
#define AIC_AWACS_ROE_YELLOW             0x000005E5L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_AWACS_ROE_WHITE
//
// MessageText:
//
//  %1, Warning White%0
//
#define AIC_AWACS_ROE_WHITE              0x000005E6L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_AWACS_WEAPONS_FREE
//
// MessageText:
//
//  %1, Weapons Free%0
//
#define AIC_AWACS_WEAPONS_FREE           0x000005E7L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_AWACS_WEAPONS_TIGHT
//
// MessageText:
//
//  %1, Weapons Tight%0
//
#define AIC_AWACS_WEAPONS_TIGHT          0x000005E8L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_AWACS_WEAPONS_SAFE
//
// MessageText:
//
//  %1,  Weapons Safe%0
//
#define AIC_AWACS_WEAPONS_SAFE           0x000005E9L

//
// %1 AWACS callsign
// Not Translated
//
// MessageId: AIC_AWACS_WEAPONS_HOLD
//
// MessageText:
//
//  %1, Weapons Hold%0
//
#define AIC_AWACS_WEAPONS_HOLD           0x000005EAL

//
// %1 CAP callsign
// Not Translated
//
// MessageId: AIC_AWACS_CAP_BUSTER_HOME
//
// MessageText:
//
//  %1, Mother under attack, buster to homeplate%0
//
#define AIC_AWACS_CAP_BUSTER_HOME        0x000005EBL

//
// %1 SEAD callsign
//
// %2 bearing
// Not Translated
//
// MessageId: AIC_SLAPSHOT_GCI
//
// MessageText:
//
//  %1, Slapshot GCI bearing%0
//
#define AIC_SLAPSHOT_GCI                 0x000005ECL

//
// %1 SEAD callsign
//
// %2 bearing
// Not Translated
//
// MessageId: AIC_SLAPSHOT_SAM
//
// MessageText:
//
//  %1, Slapshot SAM bearing%0
//
#define AIC_SLAPSHOT_SAM                 0x000005EDL

//
// %1 SEAD callsign
//
// %2 bearing
// Not Translated
//
// MessageId: AIC_SLAPSHOT_NAVAL
//
// MessageText:
//
//  %1, Slapshot Naval bearing%0
//
#define AIC_SLAPSHOT_NAVAL               0x000005EEL

//
// %1 SEAD callsign
//
// %2 bearing
// Not Translated
//
// MessageId: AIC_SLAPSHOT_AAA
//
// MessageText:
//
//  %1, Slapshot Triple A bearing%0
//
#define AIC_SLAPSHOT_AAA                 0x000005EFL

//
// %1 SEAD callsign
//
// %2 cardinal
//
// %3 range
// Not Translated
//
// MessageId: AIC_SNIPER_GCI
//
// MessageText:
//
//  %1 Sniper GCI %2 bullseye %3%0
//
#define AIC_SNIPER_GCI                   0x000005F0L

//
// %1 SEAD callsign
//
// %2 cardinal
//
// %3 range
// Not Translated
//
// MessageId: AIC_SNIPER_SAM
//
// MessageText:
//
//  %1 Sniper SAM %2 bullseye %3%0
//
#define AIC_SNIPER_SAM                   0x000005F1L

//
// %1 SEAD callsign
//
// %2 cardinal
//
// %3 range
// Not Translated
//
// MessageId: AIC_SNIPER_NAVAL
//
// MessageText:
//
//  %1 Sniper Naval %2 bullseye %3%0
//
#define AIC_SNIPER_NAVAL                 0x000005F2L

//
// %1 SEAD callsign
//
// %2 cardinal
//
// %3 range
// Not Translated
//
// MessageId: AIC_SNIPER_AAA
//
// MessageText:
//
//  %1 Sniper Triple A %2 bullseye %3%0
//
#define AIC_SNIPER_AAA                   0x000005F3L

// Not Translated
//
// MessageId: AIC_AWACS_GOOD_WORK
//
// MessageText:
//
//  Great job Sandy!  Bring 'em home!%0
//
#define AIC_AWACS_GOOD_WORK              0x000005F4L

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_USAF_HITS_FROM_GROUND
//
// MessageText:
//
//  %1 took some hits from the ground back there%0
//
#define AIC_USAF_HITS_FROM_GROUND        0x000005F5L

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_USAF_WE_HIT_HURT
//
// MessageText:
//
//  %1 hurt, assessing damage now!%0
//
#define AIC_USAF_WE_HIT_HURT             0x000005F6L

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_RAF_WE_HIT_HURT
//
// MessageText:
//
//  %1 has been hit, checking damage now!%0
//
#define AIC_RAF_WE_HIT_HURT              0x000005F7L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_WING_WE_HIT_RUSF
//
// MessageText:
//
//  %1 I have been hit!%0
//
#define AIC_WING_WE_HIT_RUSF             0x000005F8L

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_USAF_TAKING_AAA
//
// MessageText:
//
//  %1  taking heavy triple A fire!%0
//
#define AIC_USAF_TAKING_AAA              0x000005F9L

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_RAF_TAKING_AAA
//
// MessageText:
//
//  %1 is encountering murderously heavy Flak!%0
//
#define AIC_RAF_TAKING_AAA               0x000005FAL

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_RUSF_TAKING_AAA
//
// MessageText:
//
//  %1 is encountering heavy ground fire!%0
//
#define AIC_RUSF_TAKING_AAA              0x000005FBL

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_USAF_TAKING_FIRE
//
// MessageText:
//
//  %1 taking fire!%0
//
#define AIC_USAF_TAKING_FIRE             0x000005FCL

//
// %1 fighter callsign
//
// MessageId: AIC_RAF_TAKING_CANNON_FIRE
//
// MessageText:
//
//  %1, We're taking cannon fire!%0
//
#define AIC_RAF_TAKING_CANNON_FIRE       0x000005FDL

//
// %1 fighter callsign
//
// MessageId: AIC_RUSF_TAKING_CANNON_FIRE
//
// MessageText:
//
//  %1, I am taking cannon fire%0
//
#define AIC_RUSF_TAKING_CANNON_FIRE      0x000005FEL

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_RAF_MISSILE_HAD_WAY
//
// MessageText:
//
//  %1, that missile just had it's way with us!%0
//
#define AIC_RAF_MISSILE_HAD_WAY          0x000005FFL

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_RUSF_MISSILE_IMPACT
//
// MessageText:
//
//  %1, missile impact%0
//
#define AIC_RUSF_MISSILE_IMPACT          0x00000600L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_RAF_WE_HIT
//
// MessageText:
//
//  %1, Bloody hell, we have been hit!%0
//
#define AIC_RAF_WE_HIT                   0x00000601L

//
// %1 player/wingman callsign
// Not Translated
//
// MessageId: AIC_RUSF_EJECT_CALL_1
//
// MessageText:
//
//  %1 you are on fire comrade!  Get out of there!%0
//
#define AIC_RUSF_EJECT_CALL_1            0x00000602L

//
// %1 player/wingman callsign
// Not Translated
//
// MessageId: AIC_RAF_EJECT_CALL_1
//
// MessageText:
//
//  %1 you are on fire lad!  Get out of there!%0
//
#define AIC_RAF_EJECT_CALL_1             0x00000603L

//
// %1 player/wingman callsign
// Not Translated
//
// MessageId: AIC_RAF_EJECT_CALL_3
//
// MessageText:
//
//  %1 bail out laddie!  Eject!%0
//
#define AIC_RAF_EJECT_CALL_3             0x00000604L

//
// %1 player/wingman callsign
// Not Translated
//
// MessageId: AIC_RUSF_EJECT_CALL_3
//
// MessageText:
//
//  %1 eject comrade!  Eject!%0
//
#define AIC_RUSF_EJECT_CALL_3            0x00000605L

//
// %1 fighter callsign
//
// %2 enemy ID
// Not Translated
//
// MessageId: AIC_NATO_TALLY_BANDITS_ID
//
// MessageText:
//
//  %1, Tally bandits, identification %3%0
//
#define AIC_NATO_TALLY_BANDITS_ID        0x00000606L

//
// %1 fighter callsign
//
// %2 enemy ID
// Not Translated
//
// MessageId: AIC_RUSF_TALLY_BANDITS_ID
//
// MessageText:
//
//  %1, Bandits, identified as %2%0
//
#define AIC_RUSF_TALLY_BANDITS_ID        0x00000607L

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_RAF_TALLY_BANDIT
//
// MessageText:
//
//  %1Tally ho, Bandit in sight%0
//
#define AIC_RAF_TALLY_BANDIT             0x00000608L

//
// %1 fighter callsign
// Not Translated
//
// MessageId: AIC_RAF_TALLY_BANDITS
//
// MessageText:
//
//  %1, Tally ho, bandits in sight%0
//
#define AIC_RAF_TALLY_BANDITS            0x00000609L

//
// %1 is the position in the group
// Not Translated
//
// MessageId: AIC_ENGAGE_MSG_RAF
//
// MessageText:
//
//  %1, here we go then!%0
//
#define AIC_ENGAGE_MSG_RAF               0x0000060AL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_BASIC_ACK_2_RAF
//
// MessageText:
//
//  %1 will do%0
//
#define AIC_BASIC_ACK_2_RAF              0x0000060BL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_BASIC_ACK_2_RUSF
//
// MessageText:
//
//  %1 da%0
//
#define AIC_BASIC_ACK_2_RUSF             0x0000060CL

//
// %1 wingman number
// Not Translated
//
// MessageId: AIC_BASIC_NEG_2_RUSF
//
// MessageText:
//
//  %1 nyet%0
//
#define AIC_BASIC_NEG_2_RUSF             0x0000060DL

//
// %1 tanker callsign
//
// %2 flight callsign
//
// %3 tanker gives
//
// MessageId: AIC_TANKER_LOOKING_FOR_RUSF
//
// MessageText:
//
//  %1, %2, switches are safe, the nose is cold, looking for %3.%0
//
#define AIC_TANKER_LOOKING_FOR_RUSF      0x0000060EL

//
// %1 callsign of tanker
//
// %2 callsign of flight
// Not Translated
//
// MessageId: AIC_NATO_TANKER_THANKS_MSG_2
//
// MessageText:
//
//  %1, %2, thank you for the gas.%0
//
#define AIC_NATO_TANKER_THANKS_MSG_2     0x0000060FL

//
// %1 callsign of tanker
//
// %2 callsign of flight
// Not Translated
//
// MessageId: AIC_RAF_TANKER_THANKS_MSG_2
//
// MessageText:
//
//  %1, %2, thank you for the petrol.%0
//
#define AIC_RAF_TANKER_THANKS_MSG_2      0x00000610L

//
// %1 callsign of tanker
//
// %2 callsign of flight
// Not Translated
//
// MessageId: AIC_RUSF_TANKER_THANKS_MSG_2
//
// MessageText:
//
//  %1, %2, thank you for the fuel.%0
//
#define AIC_RUSF_TANKER_THANKS_MSG_2     0x00000611L

//
// %1 Wingman number
// Not Translated
//
// MessageId: AIC_RAF_SPLASH_3
//
// MessageText:
//
//  %1, got the bugger!%0
//
#define AIC_RAF_SPLASH_3                 0x00000612L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_RAF_KILL_3
//
// MessageText:
//
//  Good showl %1%0
//
#define AIC_RAF_KILL_3                   0x00000613L

//
// %1 Wingman number
// Not Translated
//
// MessageId: AIC_RUSF_SPLASH_1
//
// MessageText:
//
//  %1 my bandit has been destroyed!%0
//
#define AIC_RUSF_SPLASH_1                0x00000614L

//
// %1 Wingman number
//
// %2 number
// Not Translated
//
// MessageId: AIC_RUSF_SPLASH_2
//
// MessageText:
//
//  %1, bandit destroyed %2%0
//
#define AIC_RUSF_SPLASH_2                0x00000615L

//
// %1 Wingman number
// Not Translated
//
// MessageId: AIC_RUSF_SPLASH_3
//
// MessageText:
//
//  %1, got the cossack!%0
//
#define AIC_RUSF_SPLASH_3                0x00000616L

//
// %1 Wingman callsign
// Not Translated
//
// MessageId: AIC_RUSF_KILL_2
//
// MessageText:
//
//  %1 target eliminated%0
//
#define AIC_RUSF_KILL_2                  0x00000617L

//
// %1 Player callsign
// Not Translated
//
// MessageId: AIC_RUSF_KILL_3
//
// MessageText:
//
//  Excellent job %1%0
//
#define AIC_RUSF_KILL_3                  0x00000618L

// Not Translated
//
// MessageId: AI_GADGET_BENT_RUSF
//
// MessageText:
//
//  radar inoperative%0
//
#define AI_GADGET_BENT_RUSF              0x00000619L

// Not Translated
//
// MessageId: AI_HYDRAULICS_BENT_RUSF
//
// MessageText:
//
//  hydraulics damaged%0
//
#define AI_HYDRAULICS_BENT_RUSF          0x0000061AL

//
// %1 Wingman callsign
//
// MessageId: AIC_WINCHESTER_2
//
// MessageText:
//
//  %1 Winchester%0
//
#define AIC_WINCHESTER_2                 0x0000061BL

//
// MessageId: AIC_WINCHESTER_RUSF
//
// MessageText:
//
//  ammunition depleted%0
//
#define AIC_WINCHESTER_RUSF              0x0000061CL

//
// %1 Wingman callsign
//
// MessageId: AIC_WINCHESTER_RUSF_2
//
// MessageText:
//
//   %1 ammunition depleted%0
//
#define AIC_WINCHESTER_RUSF_2            0x0000061DL

//
// %1 Wingman number
//
// MessageId: AIC_SKOSH_RUSF
//
// MessageText:
//
//  %1 radar missiles depleted%0
//
#define AIC_SKOSH_RUSF                   0x0000061EL

//
// %1 number of missiles
//
// MessageId: AIC_HEAT_COUNT_RUSF
//
// MessageText:
//
//  %1 heat seeking%0
//
#define AIC_HEAT_COUNT_RUSF              0x0000061FL

//
// %1 wingman number
//
// MessageId: AIC_BASIC_ACK_2_RAFF
//
// MessageText:
//
//  %1 will comply%0
//
#define AIC_BASIC_ACK_2_RAFF             0x00000620L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_1_RAF
//
// MessageText:
//
//  Hello allied aircraft, this is %1%0
//
#define AIC_SHOT_DOWN_1_RAF              0x00000621L

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_HIDING_RAF
//
// MessageText:
//
//  %1, %2 the buggers are near!  Hiding%0
//
#define AIC_SHOT_DOWN_HIDING_RAF         0x00000622L

//
// %1 Player callsign
//
// %2 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_HIDING_RUSF
//
// MessageText:
//
//  %1, %2 the cossacks are near!  Hiding!%0
//
#define AIC_SHOT_DOWN_HIDING_RUSF        0x00000623L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_BEAT_UP_RUSF_2
//
// MessageText:
//
//  %1, I am injured and unable to more.%0
//
#define AIC_SHOT_DOWN_BEAT_UP_RUSF_2     0x00000624L

//
// %1 Pilot callsign
//
// MessageId: AIC_SHOT_DOWN_BEAT_UP_RAFF_2
//
// MessageText:
//
//  %1, I seem to be injured, please send help!%0
//
#define AIC_SHOT_DOWN_BEAT_UP_RAFF_2     0x00000625L

//
// %1 Pilot callsign
//
// %2 <Bandit, Bandit/s> or <enemy chopper/s>
//
// %3 o'clock position
//
// %4 high, medium, or low
//
// %5 closing/flanking/going
//
// MessageId: AIC_VISUAL_CONTACT_2
//
// MessageText:
//
//  %1 %2, %3, %4 %5%0
//
#define AIC_VISUAL_CONTACT_2             0x00000626L

//
// %1 callsign of tanker
//
// %2 callsign of flight
//
// MessageId: AIC_WSO_PRECONTACT_READY_MSG
//
// MessageText:
//
//  %1 %2 precontact.%0
//
#define AIC_WSO_PRECONTACT_READY_MSG     0x00000627L

//
// %1 AWACS callsign
//
// %2 wignman callwign
//
// %3 range
//
// %4 cardinal
//
// MessageId: AIC_WSO_SAR_CALL_2B
//
// MessageText:
//
//  %1, %2 has ejected, request immediate SAR scramble %3 %4 bullseye.%0
//
#define AIC_WSO_SAR_CALL_2B              0x00000628L

//
// %1 AWACS callsign
//
// %2 SAR callsign
//
// MessageId: AIC_SAR_GOING_DOWN_1
//
// MessageText:
//
//  %1, %2, We are going down, repeat, we are going down.%0
//
#define AIC_SAR_GOING_DOWN_1             0x00000629L

//
// %1 Pilot callsign
//
// MessageId: AIC_WSO_FENCE_IN_1
//
// MessageText:
//
//  %1 Fence in.%0
//
#define AIC_WSO_FENCE_IN_1               0x0000062AL

//
// %1 Pilot callsign
//
// MessageId: AIC_WSO_FENCE_OUT_1
//
// MessageText:
//
//  %1 Fence out.%0
//
#define AIC_WSO_FENCE_OUT_1              0x0000062BL

//
// %1 Wingman number
//
// %2 k lbs of fuel as X.X
// Not Translated
//
// MessageId: AIC_WING_FUEL_STATE
//
// MessageText:
//
//  %1 %2%0
//
#define AIC_WING_FUEL_STATE              0x0000062CL

//
// SIMULATION - STUFF ADDED FOR F/A-18 AFTER FIRST TRANSLATION PASS MADE
//
// MessageId: AIC_ESCORT_REJOIN
//
// MessageText:
//
//  Rejoin Flight (Sub-Menu)%0
//
#define AIC_ESCORT_REJOIN                0x0000062DL

//
// %1 Flight position
//
// %2 bullseye or tactical (bra) info
// %1 %2
//
// MessageId: AIC_FLIGHT_POSITION_INFO
//
// MessageText:
//
//  %1 %2%0
//
#define AIC_FLIGHT_POSITION_INFO         0x0000062EL

//
// %1 Flight position or callsign
//
// MessageId: AI_BINGO_FUEL2
//
// MessageText:
//
//  %1 bingo fuel%0
//
#define AI_BINGO_FUEL2                   0x0000062FL

//
// %1 Flight position or callsign
//
// MessageId: AIC_BANDIT_BANDIT_2
//
// MessageText:
//
//  %1 bandit bandit%0
//
#define AIC_BANDIT_BANDIT_2              0x00000630L

//
// %1 Wingman callsign or number
//
// MessageId: AIC_COMM_SAM_LAUNCH
//
// MessageText:
//
//  %1, SAM launch%!%0
//
#define AIC_COMM_SAM_LAUNCH              0x00000631L

//
// %1Flight callsign
//
// MessageId: AIC_COMM_MISSILE_LAUNCH
//
// MessageText:
//
//  %1 Heads up!  Missile Launch%!%0
//
#define AIC_COMM_MISSILE_LAUNCH          0x00000632L

//
// %1 Wingman number
//
// MessageId: AIC_COMM_SPIKE_SPIKE
//
// MessageText:
//
//  %1 Spike Spike%0
//
#define AIC_COMM_SPIKE_SPIKE             0x00000633L

//
// %1 bandit contact beginning
// Not Translated
//
// MessageId: AIC_DECLARE2
//
// MessageText:
//
//  %1 declare.%0
//
#define AIC_DECLARE2                     0x00000634L

// Not Translated
//
// MessageId: AIC_ADV_THROTTLE_MAX
//
// MessageText:
//
//  Set throttles to maximum%0
//
#define AIC_ADV_THROTTLE_MAX             0x00000635L

// Not Translated
//
// MessageId: AIC_ADV_INCREASE_POWER
//
// MessageText:
//
//  Increase power%0
//
#define AIC_ADV_INCREASE_POWER           0x00000636L

// Not Translated
//
// MessageId: AIC_ADV_GET_SPEED_UP
//
// MessageText:
//
//  Get your speed up%0
//
#define AIC_ADV_GET_SPEED_UP             0x00000637L

// Not Translated
//
// MessageId: AIC_ADV_TAKEOFF_SPD
//
// MessageText:
//
//  Your speed should be at least one hundred and fifty knots to take off.%0
//
#define AIC_ADV_TAKEOFF_SPD              0x00000638L

// Not Translated
//
// MessageId: AIC_ADV_THROTTLES_GOOD
//
// MessageText:
//
//  Good, throttles are currently at maximum power.%0
//
#define AIC_ADV_THROTTLES_GOOD           0x00000639L

// Not Translated
//
// MessageId: AIC_ADV_RELEASE_BRAKES
//
// MessageText:
//
//  Press the B key to release the wheel brakes.%0
//
#define AIC_ADV_RELEASE_BRAKES           0x0000063AL

// Not Translated
//
// MessageId: AIC_ADV_GENTLY_BACK
//
// MessageText:
//
//  Okay, gently pull back on the stick%0
//
#define AIC_ADV_GENTLY_BACK              0x0000063BL

// Not Translated
//
// MessageId: AIC_ADV_GENTLY_BACK_2
//
// MessageText:
//
//  Gently pull back on the stick%0
//
#define AIC_ADV_GENTLY_BACK_2            0x0000063CL

// Not Translated
//
// MessageId: AIC_ADV_PULL_BACK
//
// MessageText:
//
//  Pull back on the stick%0
//
#define AIC_ADV_PULL_BACK                0x0000063DL

// Not Translated
//
// MessageId: AIC_ADV_INC_ALT
//
// MessageText:
//
//  Increase your altitude%0
//
#define AIC_ADV_INC_ALT                  0x0000063EL

// Not Translated
//
// MessageId: AIC_ADV_WATCH_ALT
//
// MessageText:
//
//  Watch your altitude%0
//
#define AIC_ADV_WATCH_ALT                0x0000063FL

// Not Translated
//
// MessageId: AIC_ADV_POS_CLIMB
//
// MessageText:
//
//  Maintain a positive rate of climb%0
//
#define AIC_ADV_POS_CLIMB                0x00000640L

// Not Translated
//
// MessageId: AIC_ADV_RAISE_GEAR
//
// MessageText:
//
//  Raise your landing gear%0
//
#define AIC_ADV_RAISE_GEAR               0x00000641L

// Not Translated
//
// MessageId: AIC_ADV_GEAR_UP
//
// MessageText:
//
//  Put your landing gear up%0
//
#define AIC_ADV_GEAR_UP                  0x00000642L

// Not Translated
//
// MessageId: AIC_ADV_SIGNAL_CAT
//
// MessageText:
//
//  Signal the catapult officer when you are ready to takeoff by pressing the B key.%0
//
#define AIC_ADV_SIGNAL_CAT               0x00000643L

// Not Translated
//
// MessageId: AIC_ADV_NO_TOUCH
//
// MessageText:
//
//  During launch, do not touch the controls until you have cleared the deck.%0
//
#define AIC_ADV_NO_TOUCH                 0x00000644L

// Not Translated
//
// MessageId: AIC_ADV_TARGET_IN_RANGE
//
// MessageText:
//
//  Target is now in range%0
//
#define AIC_ADV_TARGET_IN_RANGE          0x00000645L

// Not Translated
//
// MessageId: AIC_ADV_LAUNCH_WEAPON
//
// MessageText:
//
//  Target now in range, launch your weapon%0
//
#define AIC_ADV_LAUNCH_WEAPON            0x00000646L

// Not Translated
//
// MessageId: AIC_ADV_LAUNCH_WEAPON_2
//
// MessageText:
//
//  Target is now in range, you can take the shot%0
//
#define AIC_ADV_LAUNCH_WEAPON_2          0x00000647L

// Not Translated
//
// MessageId: AIC_ADV_TARGET_IN_RANGE_2
//
// MessageText:
//
//  Target in range%0
//
#define AIC_ADV_TARGET_IN_RANGE_2        0x00000648L

// Not Translated
//
// MessageId: AIC_ADV_TARGET_TOO_CLOSE
//
// MessageText:
//
//  Target is too close%0
//
#define AIC_ADV_TARGET_TOO_CLOSE         0x00000649L

// Not Translated
//
// MessageId: AIC_ADV_TARGET_TOO_FAR
//
// MessageText:
//
//  Target is too far away%0
//
#define AIC_ADV_TARGET_TOO_FAR           0x0000064AL

// Not Translated
//
// MessageId: AIC_ADV_WAYPT_BEHIND
//
// MessageText:
//
//  Waypoint is behind you%0
//
#define AIC_ADV_WAYPT_BEHIND             0x0000064BL

// Not Translated
//
// MessageId: AIC_ADV_WAYPT_LEFT
//
// MessageText:
//
//  Waypoint is to your left%0
//
#define AIC_ADV_WAYPT_LEFT               0x0000064CL

// Not Translated
//
// MessageId: AIC_ADV_WAYPT_RIGHT
//
// MessageText:
//
//  Waypoint is to your right%0
//
#define AIC_ADV_WAYPT_RIGHT              0x0000064DL

// Not Translated
//
// MessageId: AIC_ADV_WAYPT_FRONT
//
// MessageText:
//
//  Waypoint is in front of you.%0
//
#define AIC_ADV_WAYPT_FRONT              0x0000064EL

// Not Translated
//
// MessageId: AIC_ADV_TARGET_BEHIND
//
// MessageText:
//
//  Target is behind you%0
//
#define AIC_ADV_TARGET_BEHIND            0x0000064FL

// Not Translated
//
// MessageId: AIC_ADV_TARGET_FRONT
//
// MessageText:
//
//  Target is in front of you%0
//
#define AIC_ADV_TARGET_FRONT             0x00000650L

// Not Translated
//
// MessageId: AIC_ADV_TARGET_LEFT
//
// MessageText:
//
//  Target is to your left%0
//
#define AIC_ADV_TARGET_LEFT              0x00000651L

// Not Translated
//
// MessageId: AIC_ADV_TARGET_RIGHT
//
// MessageText:
//
//  Target is to your right%0
//
#define AIC_ADV_TARGET_RIGHT             0x00000652L

// Not Translated
//
// MessageId: AIC_ADV_TARGET_ABOVE
//
// MessageText:
//
//  Target is above you%0
//
#define AIC_ADV_TARGET_ABOVE             0x00000653L

// Not Translated
//
// MessageId: AIC_ADV_TARGET_BELOW
//
// MessageText:
//
//  Target is below you%0
//
#define AIC_ADV_TARGET_BELOW             0x00000654L

//
// %1 Name of Player with different database
// Not Translated
//
// MessageId: AIC_NET_HACK_DETECTED
//
// MessageText:
//
//  WARNING!  Player %1's database is different than yours.%0
//
#define AIC_NET_HACK_DETECTED            0x00000655L

// Not Translated
//
// MessageId: AIC_HOST_END_NOTIFY
//
// MessageText:
//
//  Host has exited mission%0
//
#define AIC_HOST_END_NOTIFY              0x00000656L

//
// I've used the 'Enter' key.  You could use 'Y'es or for German 'J'a.
// Not Translated
//
// MessageId: AIC_FORCE_END
//
// MessageText:
//
//  Use Enter key to end mission.%0
//
#define AIC_FORCE_END                    0x00000657L

//
// SIMULATION - PILOT AIDS
//
// NOTE FOR GERMAN.  The font for these phrases doesn't have the - character so I switched it to A instead.  In the phrases below if you see AR it should be -R.  SRE 11/12/99
// Not Translated
//
// MessageId: MSG_PILOTAID_PRIMARY
//
// MessageText:
//
//  PRIMARY TARGET%0
//
#define MSG_PILOTAID_PRIMARY             0x00000658L

// Not Translated
//
// MessageId: MSG_PILOTAID_SECONDARY
//
// MessageText:
//
//  SECONDARY TARGET%0
//
#define MSG_PILOTAID_SECONDARY           0x00000659L

// Not Translated
//
// MessageId: MSG_PILOTAID_TARGET
//
// MessageText:
//
//  TARGET%0
//
#define MSG_PILOTAID_TARGET              0x0000065AL

// Not Translated
//
// MessageId: MSG_PILOTAID_PRIMARY_OBJ
//
// MessageText:
//
//  PRIMARY_OBJECTIVE%0
//
#define MSG_PILOTAID_PRIMARY_OBJ         0x0000065BL

// Not Translated
//
// MessageId: MSG_PILOTAID_SECOND_OBJ
//
// MessageText:
//
//  SECONDARY_OBJECTIVE%0
//
#define MSG_PILOTAID_SECOND_OBJ          0x0000065CL

//
// SIMULATION - PILOT AIDS 10/7/99
// Not Translated
//
// MessageId: MSG_PILOTAID_SAM
//
// MessageText:
//
//  SAM%0
//
#define MSG_PILOTAID_SAM                 0x0000065DL

// Not Translated
//
// MessageId: MSG_PILOTAID_RDR_SITE
//
// MessageText:
//
//  RDR SITE%0
//
#define MSG_PILOTAID_RDR_SITE            0x0000065EL

// Not Translated
//
// MessageId: MSG_PILOTAID_VEHICLE
//
// MessageText:
//
//  VEHICLE%0
//
#define MSG_PILOTAID_VEHICLE             0x0000065FL

// Not Translated
//
// MessageId: MSG_PILOTAID_RUNWAY
//
// MessageText:
//
//  RUNWAY%0
//
#define MSG_PILOTAID_RUNWAY              0x00000660L

// Not Translated
//
// MessageId: MSG_PILOTAID_GROUND_TARGET
//
// MessageText:
//
//  GROUND TARGET%0
//
#define MSG_PILOTAID_GROUND_TARGET       0x00000661L

// Not Translated
//
// MessageId: MSG_PILOTAID_HIT_DESTRUCTO
//
// MessageText:
//
//  Hit Destructo Cam Now%0
//
#define MSG_PILOTAID_HIT_DESTRUCTO       0x00000662L

// Not Translated
//
// MessageId: MSG_PILOTAID_HIT_DESTRUCTO_RET
//
// MessageText:
//
//  Hit Destructo Cam To Return%0
//
#define MSG_PILOTAID_HIT_DESTRUCTO_RET   0x00000663L

// Not Translated
//
// MessageId: MSG_PILOTAID_BULLSEYE
//
// MessageText:
//
//  BULLSEYE%0
//
#define MSG_PILOTAID_BULLSEYE            0x00000664L

// Not Translated
//
// MessageId: MSG_PILOTAID_WAYPOINT
//
// MessageText:
//
//  WAYPOINT%0
//
#define MSG_PILOTAID_WAYPOINT            0x00000665L

//
// SIMULATION - FLIGHT STATISTICS
//
// MessageId: DEBRIEF_STAT_FLIGHTSTATISTICS
//
// MessageText:
//
//  Flight Statistics%0
//
#define DEBRIEF_STAT_FLIGHTSTATISTICS    0x00000666L

//
// MessageId: DEBRIEF_STAT_MAXG
//
// MessageText:
//
//  Maximum G%0
//
#define DEBRIEF_STAT_MAXG                0x00000667L

//
// MessageId: DEBRIEF_STAT_MAXSPEED
//
// MessageText:
//
//  Maximum Speed (Knots)%0
//
#define DEBRIEF_STAT_MAXSPEED            0x00000668L

//
// MessageId: DEBRIEF_STAT_MAXALT
//
// MessageText:
//
//  Maximum Altitude%0
//
#define DEBRIEF_STAT_MAXALT              0x00000669L

//
// MessageId: DEBRIEF_STAT_MINALT
//
// MessageText:
//
//  Minimum Altitude (Gear Up, Speed >300 KIAS)%0
//
#define DEBRIEF_STAT_MINALT              0x0000066AL

//
// MessageId: DEBRIEF_STAT_FUELUSED
//
// MessageText:
//
//  Total Fuel Used%0
//
#define DEBRIEF_STAT_FUELUSED            0x0000066BL

//
// MessageId: DEBRIEF_STAT_STARTTIME
//
// MessageText:
//
//  Mission Start Time%0
//
#define DEBRIEF_STAT_STARTTIME           0x0000066CL

//
// MessageId: DEBRIEF_STAT_MISSIONTIME
//
// MessageText:
//
//  Total Mission Time%0
//
#define DEBRIEF_STAT_MISSIONTIME         0x0000066DL

//
// MessageId: DEBRIEF_STAT_TAKEOFF_TIME
//
// MessageText:
//
//  Take-Off Time%0
//
#define DEBRIEF_STAT_TAKEOFF_TIME        0x0000066EL

//
// MessageId: DEBRIEF_STAT_REFUEL_TIME
//
// MessageText:
//
//  Refueling Time%0
//
#define DEBRIEF_STAT_REFUEL_TIME         0x0000066FL

//
// MessageId: DEBRIEF_STAT_FENCEIN_TIME
//
// MessageText:
//
//  Fence-In Time%0
//
#define DEBRIEF_STAT_FENCEIN_TIME        0x00000670L

//
// MessageId: DEBRIEF_STAT_FENCEOUT_TIME
//
// MessageText:
//
//  Fence-Out Time%0
//
#define DEBRIEF_STAT_FENCEOUT_TIME       0x00000671L

//
// MessageId: DEBRIEF_STAT_LANDING_TIME
//
// MessageText:
//
//  Landing Time%0
//
#define DEBRIEF_STAT_LANDING_TIME        0x00000672L

//
// MessageId: DEBRIEF_STAT_AIRTOAIR
//
// MessageText:
//
//  Air-To-Air Weapon Statistics%0
//
#define DEBRIEF_STAT_AIRTOAIR            0x00000673L

//
// MessageId: DEBRIEF_STAT_GUNROUNDSFIRED
//
// MessageText:
//
//  Gun Rounds Fired%0
//
#define DEBRIEF_STAT_GUNROUNDSFIRED      0x00000674L

//
// MessageId: DEBRIEF_STAT_GUNROUNDSHIT
//
// MessageText:
//
//  Gun Rounds Hit%0
//
#define DEBRIEF_STAT_GUNROUNDSHIT        0x00000675L

//
// MessageId: DEBRIEF_STAT_GUNROUNDKILLS
//
// MessageText:
//
//  Gun Round Kills%0
//
#define DEBRIEF_STAT_GUNROUNDKILLS       0x00000676L

//
// MessageId: DEBRIEF_STAT_AAMISSILESFIRED
//
// MessageText:
//
//  Air-To-Air Missiles Fired%0
//
#define DEBRIEF_STAT_AAMISSILESFIRED     0x00000677L

//
// MessageId: DEBRIEF_STAT_AAMISSILESHIT
//
// MessageText:
//
//  Air-To-Air Missile Hits%0
//
#define DEBRIEF_STAT_AAMISSILESHIT       0x00000678L

//
// MessageId: DEBRIEF_STAT_AAMISSILEKILLS
//
// MessageText:
//
//  Air-To-Air Missile Kills%0
//
#define DEBRIEF_STAT_AAMISSILEKILLS      0x00000679L

//
// MessageId: DEBRIEF_STAT_ENEMYAIRDESTROYED
//
// MessageText:
//
//  Enemy Aircraft Destroyed%0
//
#define DEBRIEF_STAT_ENEMYAIRDESTROYED   0x0000067AL

//
// MessageId: DEBRIEF_STAT_FRIENDLYAIRDESTROYED
//
// MessageText:
//
//  Friendly Aircraft Destroyed%0
//
#define DEBRIEF_STAT_FRIENDLYAIRDESTROYED 0x0000067BL

//
// MessageId: DEBRIEF_STAT_NEUTRALAIRDESTROYED
//
// MessageText:
//
//  Neutral Aircraft Destroyed%0
//
#define DEBRIEF_STAT_NEUTRALAIRDESTROYED 0x0000067CL

//
// MessageId: DEBIREF_STAT_AAHITRATIO
//
// MessageText:
//
//  Air-To-Air Missile Hit Ratio%0
//
#define DEBIREF_STAT_AAHITRATIO          0x0000067DL

//
// MessageId: DEBRIEF_STAT_AAMISSILEKILLRATIO
//
// MessageText:
//
//  Air-To-Air Missile Kill Ratio%0
//
#define DEBRIEF_STAT_AAMISSILEKILLRATIO  0x0000067EL

//
// MessageId: DEBRIEF_STAT_GUNROUNDHITRATIO
//
// MessageText:
//
//  Gun Round Hit Ratio%0
//
#define DEBRIEF_STAT_GUNROUNDHITRATIO    0x0000067FL

//
// MessageId: DEBRIEF_STAT_GUNROUNDKILLRATIO
//
// MessageText:
//
//  Gun Round Kill Ratio%0
//
#define DEBRIEF_STAT_GUNROUNDKILLRATIO   0x00000680L

//
// MessageId: DEBRIEF_STAT_AIRTOGROUND
//
// MessageText:
//
//  Air-To-Ground Weapon Statistics%0
//
#define DEBRIEF_STAT_AIRTOGROUND         0x00000681L

//
// MessageId: DEBRIEF_STAT_DUMBBOMBSRELEASED
//
// MessageText:
//
//  Dumb Bombs Released%0
//
#define DEBRIEF_STAT_DUMBBOMBSRELEASED   0x00000682L

//
// MessageId: DEBRIEF_STAT_DUMBBOMBHITS
//
// MessageText:
//
//  Dumb Bomb Hits (Includes Proximity Hit Damage)%0
//
#define DEBRIEF_STAT_DUMBBOMBHITS        0x00000683L

//
// MessageId: DEBRIEF_STAT_DUMBBOMBKILLS
//
// MessageText:
//
//  Dumb Bomb Kills%0
//
#define DEBRIEF_STAT_DUMBBOMBKILLS       0x00000684L

//
// MessageId: DEBRIEF_STAT_AGMISSILESRELEASED
//
// MessageText:
//
//  Air-To-Ground Missiles Released%0
//
#define DEBRIEF_STAT_AGMISSILESRELEASED  0x00000685L

//
// MessageId: DEBRIEF_STAT_AGMISSILEHITS
//
// MessageText:
//
//  Air-To-Ground Missiles Hits (Includes Proximity Hit Damage)%0
//
#define DEBRIEF_STAT_AGMISSILEHITS       0x00000686L

//
// MessageId: DEBRIEF_STAT_AGMMISSILEKILLS
//
// MessageText:
//
//  Air-To-Ground Missile Kills%0
//
#define DEBRIEF_STAT_AGMMISSILEKILLS     0x00000687L

//
// MessageId: DEBRIEF_STAT_GROUNDKILLS
//
// MessageText:
//
//  Ground Target Kills%0
//
#define DEBRIEF_STAT_GROUNDKILLS         0x00000688L

//
// MessageId: DEBRIEF_STAT_MOVINGVEHILCEKILLS
//
// MessageText:
//
//  Moving Vehicle Kills%0
//
#define DEBRIEF_STAT_MOVINGVEHILCEKILLS  0x00000689L

//
// MessageId: DEBRIEF_STAT_AGBOMBHITRATIO
//
// MessageText:
//
//  Air-To-Ground Bomb Hit Ratio%0
//
#define DEBRIEF_STAT_AGBOMBHITRATIO      0x0000068AL

//
// MessageId: DEBRIEF_STAT_AGBOMBKILLRATIO
//
// MessageText:
//
//  Air-To-Ground Bomb Kill Ratio%0
//
#define DEBRIEF_STAT_AGBOMBKILLRATIO     0x0000068BL

//
// MessageId: DEBRIEF_STAT_AGMISSILEHITRATIO
//
// MessageText:
//
//  Air-To-Ground Missile Hit Ratio%0
//
#define DEBRIEF_STAT_AGMISSILEHITRATIO   0x0000068CL

//
// MessageId: DEBRIEF_STAT_AGMISSILEKILLRATIO
//
// MessageText:
//
//  Air-To-Ground Missile Kill Ratio%0
//
#define DEBRIEF_STAT_AGMISSILEKILLRATIO  0x0000068DL

//
// MessageId: DEBRIEF_STAT_FLIGHTCREWSTATISTICS
//
// MessageText:
//
//  Flight Crew Statistics%0
//
#define DEBRIEF_STAT_FLIGHTCREWSTATISTICS 0x0000068EL

//
// MessageId: DEBRIEF_STAT_FLIGHTCREWKIA
//
// MessageText:
//
//  Flight Crew KIA%0
//
#define DEBRIEF_STAT_FLIGHTCREWKIA       0x0000068FL

//
// MessageId: DEBRIEF_STAT_FLIGHTVREWMIA
//
// MessageText:
//
//  Flight Crew MIA%0
//
#define DEBRIEF_STAT_FLIGHTVREWMIA       0x00000690L

//
// MessageId: DEBRIEF_STAT_FLIGHTCREWSURVIVED
//
// MessageText:
//
//  Flight Crew Survived%0
//
#define DEBRIEF_STAT_FLIGHTCREWSURVIVED  0x00000691L

//
// MessageId: DEBRIEF_STAT_FLIGHTEJECTENEMY
//
// MessageText:
//
//  Flight Crew Ejected In Enemy Territory%0
//
#define DEBRIEF_STAT_FLIGHTEJECTENEMY    0x00000692L

//
// MessageId: DEBRIEF_STAT_FLIGHTEJECTFRIENDLY
//
// MessageText:
//
//  Flight Crew Ejected In Friendly Territory%0
//
#define DEBRIEF_STAT_FLIGHTEJECTFRIENDLY 0x00000693L

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_OWN_BOMB
//
// MessageText:
//
//  Shotdown by your own bombs frag!%0
//
#define DEBRIEF_STAT_SHOTDOWN_OWN_BOMB   0x00000694L

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_ENEMY_AAA
//
// MessageText:
//
//  Shotdown by Enemy AAA%0
//
#define DEBRIEF_STAT_SHOTDOWN_ENEMY_AAA  0x00000695L

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_ENEMY_SAM
//
// MessageText:
//
//  Shotdown by Enemy SAM%0
//
#define DEBRIEF_STAT_SHOTDOWN_ENEMY_SAM  0x00000696L

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_ENEMY_MISSILE
//
// MessageText:
//
//  Shotdown by Enemy Missile%0
//
#define DEBRIEF_STAT_SHOTDOWN_ENEMY_MISSILE 0x00000697L

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_ENEMY_GUNS
//
// MessageText:
//
//  Shotdown by Enemy Guns%0
//
#define DEBRIEF_STAT_SHOTDOWN_ENEMY_GUNS 0x00000698L

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_ENEMY_BOMB
//
// MessageText:
//
//  Shotdown by Enemy Bomb%0
//
#define DEBRIEF_STAT_SHOTDOWN_ENEMY_BOMB 0x00000699L

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_ENEMY_FRAG
//
// MessageText:
//
//  Shotdown by Enemy Bomb Frag%0
//
#define DEBRIEF_STAT_SHOTDOWN_ENEMY_FRAG 0x0000069AL

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_FRIEND_AAA
//
// MessageText:
//
//  Shotdown by Friendly AAA%0
//
#define DEBRIEF_STAT_SHOTDOWN_FRIEND_AAA 0x0000069BL

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_FRIEND_SAM
//
// MessageText:
//
//  Shotdown by Friendly SAM%0
//
#define DEBRIEF_STAT_SHOTDOWN_FRIEND_SAM 0x0000069CL

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_FRIEND_MISSILE
//
// MessageText:
//
//  Shotdown by Friendly Missile%0
//
#define DEBRIEF_STAT_SHOTDOWN_FRIEND_MISSILE 0x0000069DL

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_FRIEND_GUNS
//
// MessageText:
//
//  Shotdown by Friendly Guns%0
//
#define DEBRIEF_STAT_SHOTDOWN_FRIEND_GUNS 0x0000069EL

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_FRIEND_BOMB
//
// MessageText:
//
//  Shotdown by Friendly Bomb%0
//
#define DEBRIEF_STAT_SHOTDOWN_FRIEND_BOMB 0x0000069FL

//
// MessageId: DEBRIEF_STAT_SHOTDOWN_FREND_FRAG
//
// MessageText:
//
//  Shotdown by Friendly Bomb Frag%0
//
#define DEBRIEF_STAT_SHOTDOWN_FREND_FRAG 0x000006A0L

// Refers To Flight Element Number - Flight #1 (Aircraft #1)
//
// MessageId: DEBRIEF_STAT_FLIGHT
//
// MessageText:
//
//  Flight #%0
//
#define DEBRIEF_STAT_FLIGHT              0x000006A1L

//
// MessageId: DEBRIEF_STAT_FLIGHTSTATUS
//
// MessageText:
//
//  Flight Status%0
//
#define DEBRIEF_STAT_FLIGHTSTATUS        0x000006A2L

//
// MessageId: DEBRIEF_STAT_AIRCRAFT_DAMAGED
//
// MessageText:
//
//  Aircraft Damaged%0
//
#define DEBRIEF_STAT_AIRCRAFT_DAMAGED    0x000006A3L

//
// MessageId: DEBRIEF_STAT_AIRCRAFT_DESTROYED
//
// MessageText:
//
//  Aircraft Destroyed%0
//
#define DEBRIEF_STAT_AIRCRAFT_DESTROYED  0x000006A4L

//
// MessageId: DEBRIEF_STAT_AIRCRAFT_OPERATIONAL
//
// MessageText:
//
//  Aircraft Operational%0
//
#define DEBRIEF_STAT_AIRCRAFT_OPERATIONAL 0x000006A5L

// Summary information for entire flight of aircraft
//
// MessageId: DEBRIEF_STAT_FLIGHT_TOTALS
//
// MessageText:
//
//  Flight Totals%0
//
#define DEBRIEF_STAT_FLIGHT_TOTALS       0x000006A6L

// Displayed On HUD when you crash
//
// MessageId: STATUS_MESSAGE_CRASHED
//
// MessageText:
//
//  CRASHED%0
//
#define STATUS_MESSAGE_CRASHED           0x000006A7L

//
// MessageId: STATUS_MESSAGE_LOADINGMISSION
//
// MessageText:
//
//  Loading Mission…%0
//
#define STATUS_MESSAGE_LOADINGMISSION    0x000006A8L

//
// MessageId: STATUS_MESSAGE_LOADINGWORLDELEV
//
// MessageText:
//
//  Loading World Elevation Data…%0
//
#define STATUS_MESSAGE_LOADINGWORLDELEV  0x000006A9L

//
// MessageId: STATUS_MESSAGE_LOADINGTERRAINFEATURE
//
// MessageText:
//
//  Loading Terrain Feature Set…%0
//
#define STATUS_MESSAGE_LOADINGTERRAINFEATURE 0x000006AAL

//
// MessageId: STATUS_MESSAGE_LOADINGAIRCRAFT
//
// MessageText:
//
//  Loading Aircraft Database…%0
//
#define STATUS_MESSAGE_LOADINGAIRCRAFT   0x000006ABL

//
// MessageId: STATUS_MESSAGE_LOADINGWEAPONS
//
// MessageText:
//
//  Loading Weapons Database…%0
//
#define STATUS_MESSAGE_LOADINGWEAPONS    0x000006ACL

//
// MessageId: STATUS_MESSAGE_LOADINGGAMESETTINGS
//
// MessageText:
//
//  Loading Game Settings…%0
//
#define STATUS_MESSAGE_LOADINGGAMESETTINGS 0x000006ADL

//
// MessageId: STATUS_MESSAGE_PREPARINGAIRCRAFT
//
// MessageText:
//
//  Preparing Aircraft…%0
//
#define STATUS_MESSAGE_PREPARINGAIRCRAFT 0x000006AEL

//
// MessageId: STATUS_MESSAGE_ERRORFILENOTFOUND
//
// MessageText:
//
//  Error - File Not Found%0
//
#define STATUS_MESSAGE_ERRORFILENOTFOUND 0x000006AFL

//
// SIMULATION - SOUND
//
// SOUND DEFINES
//
// Planenum is the first arguement passed for the general text function.
//
// Targetnum is the second argument passed for the general text function.
//
//  
//
// v1 Callsign
//
// v2 Callsign with Number
//
// v3 Number in group
//
// v4 Callsign intense
//
// v5 Callsign with Number intense
//
// v6 Number in group intense.
//
// v7 Bearing to target
//
// v8 cardinal direction
//
// v9 cardinal direction 2 <bullseye, or ern extension ie. Eastern> 
//
// v10 Range to target in NM
//
// v11 High/Medium/Low
//
// v12 Seconds til impact
//
// v13 Altitude <High / Medium / Low>
//
// v14 <bandit bandit> or <unkown>
//
// v15 closing/flanking/going away
//
// v16 Seconds til release.
//
// v17 Seconds til pull.
//
// v18 Minutes to Nav Point.
//
// v19 Miles to Target.
//
// v20 <group / single / bandit>
//
// v21 <group bearing / single bearing>
//
// v22 <additional> <group>/<single> <bearing>
//
// v23 left/right
//
// v24 o'clock position of enemy
//
// v25 callsign of flight off planenum.
//
// v26 Callsign of flight off targetnum.
//
// v27 Callsign off targetnum
//
// v28 Callsign with Number off targetnum.
//
// v29 Number in group off targetnum.
//
// v30 Callsign off targetnum intense.
//
// v31 Callsign with Number off targetnum intense.
//
// v32 Number in group off targetnum intense.
//
// v33 Distance remaining in ft, increments of 10.
//
// v34 Ground callsign off planenum.
//
// v35 Ground callsign off targetnum.
//
// v36 Aspect based off of targetnum.
//
// v37 Clock position intense.
//
// v38 Fast/slow mover.
//
// v39 Altitude in angels
//
// v40 Altitude in thousand
//
// v41 Spotted visual
//
// v42 Fuel state of planenum
//
// v43 Bullseye off headingval2
//
// v44 Digital Bullseye off headingval2
//
// 
//
// AI COMMANDS
//
// v3 position in group
//
// r2 disengagement message
// Not Translated
//
// MessageId: AICF_DISENGAGE_MSG
//
// MessageText:
//
//  v3 r2%0
//
#define AICF_DISENGAGE_MSG               0x000006B0L

//
// v3 position in group
//
// 635 <unable to comply> message
// v3 635
//
// MessageId: AICF_NO_JOY_MSG
//
// MessageText:
//
//  v3 635%0
//
#define AICF_NO_JOY_MSG                  0x000006B1L

//
// v6 position in group (intense)
//
// r2 engaged offensive message
// Not Translated
//
// MessageId: AICF_ENGAGE_MSG
//
// MessageText:
//
//  v6 r2 %0
//
#define AICF_ENGAGE_MSG                  0x000006B2L

//
// v3 position in group
//
// 700 <in hot> message
// v3 700
//
// MessageId: AICF_IN_HOT
//
// MessageText:
//
//  v3 700%0
//
#define AICF_IN_HOT                      0x000006B3L

//
// v3 position in group
//
// r2 bomb drop message
// Not Translated
//
// MessageId: AICF_BOMB_DROP
//
// MessageText:
//
//  v3 r2%0
//
#define AICF_BOMB_DROP                   0x000006B4L

//
// v3 position in group
//
// r2 bomb drop message
// Not Translated
//
// MessageId: AICF_MAVERICK_LAUNCH
//
// MessageText:
//
//  v3 r2%0
//
#define AICF_MAVERICK_LAUNCH             0x000006B5L

//
// v3 position in group
//
// 701 <off to the> message
//
// v8 cardinal direction
// v3 701 v8
//
// MessageId: AICF_END_GRND_ATTACK
//
// MessageText:
//
//  v3 701 v8%0
//
#define AICF_END_GRND_ATTACK             0x000006B6L

//
// v3 position in group
//
// v21 group/single <bearing>
//
// v7 bearing to target
//
// v10 range to target
//
// v11 high/medium/low
//
// v8 targets heading
//
// v14 <bandit bandit> or <unkown>
// Not Translated
//
// MessageId: AICF_RADAR_CONTACT
//
// MessageText:
//
//  v3 v21 v7 v10 v11 v8 v14%0
//
#define AICF_RADAR_CONTACT               0x000006B7L

//
// v3 position in group
//
// v14 <bandit bandit> or <unkown>
//
// v23 left/right
//
// v24 o'clock position of enemy
//
// v10 range to target
//
// v15 closing/flanking/going away
// Not Translated
//
// MessageId: AICF_VISUAL_CONTACT
//
// MessageText:
//
//  v3 v14 v23 v24 v10 v15%0
//
#define AICF_VISUAL_CONTACT              0x000006B8L

//
// v3 position in group
//
// v21 <group>/<single> bearing
//
// v7 bearing
//
// v10 range to target
//
// v11 high/level/low
//
// v8 targets heading
//
// v14 <bandit bandit> or <unkown>
// Not Translated
//
// MessageId: AICF_CONTACT_CALL
//
// MessageText:
//
//  v3 v21 v7 v10 v11 v8 v14 %0
//
#define AICF_CONTACT_CALL                0x000006B9L

//
// v3 position in group
//
// v22 <additional> <group>/<single> <bearing>
//
// v7 bearing
//
// v10 range to target
//
// v11 high/level/low
//
// v8 targets heading
// Not Translated
//
// MessageId: AICF_ADD_CONTACT
//
// MessageText:
//
//  v3 v22 v7 v10 v11 v8%0
//
#define AICF_ADD_CONTACT                 0x000006BAL

//
// v3 position in group
//
// 583 <contact>
//
// v21 <group>/<single> <bearing>
//
// v7 bearing
//
// v10 range to target
//
// v11 high/level/low
//
// v8 targets heading
// v3 583 v21 v7 v10 v11 v8
//
// MessageId: AICF_NL_CONTACT
//
// MessageText:
//
//  v3 583 v21 v7 v10 v11 v8%0
//
#define AICF_NL_CONTACT                  0x000006BBL

//
// v2 Pilot callsign
//
// 581 <going Hot>
// v2 581
//
// MessageId: AICF_GOING_HOT_MSG
//
// MessageText:
//
//  v2 581%0
//
#define AICF_GOING_HOT_MSG               0x000006BCL

//
// v2 Pilot callsign
//
// 582 <going Cold>
// v2 582
//
// MessageId: AICF_GOING_COLD_MSG
//
// MessageText:
//
//  v2 582%0
//
#define AICF_GOING_COLD_MSG              0x000006BDL

//
// v3 position in group
//
// 586 <clean>
// v3 586
//
// MessageId: AICF_CLEAN_MSG
//
// MessageText:
//
//  v3 586%0
//
#define AICF_CLEAN_MSG                   0x000006BEL

//
// v27 callsign of tanker
//
// v25 callsign of flight
//
// 508 <request clearance to precontact>
// Not Translated
//
// MessageId: AICF_WSO_REQUEST_TANKER_MSG
//
// MessageText:
//
//  v27 v25 508%0
//
#define AICF_WSO_REQUEST_TANKER_MSG      0x000006BFL

//
// v28 callsign of tanker
//
// v2 Pilot callsign
//
// 659 <precontact>
// v27 v1 659
//
// MessageId: AICF_WSO_PRECONTACT_READY_MSG
//
// MessageText:
//
//  v27 v1 659%0
//
#define AICF_WSO_PRECONTACT_READY_MSG    0x000006C0L

//
// v28 callsign of tanker
//
// v2 callsign of flight
//
// 660 <Receiving Fuel>
// v28 v2 660
//
// MessageId: AICF_WSO_FUELING_MSG
//
// MessageText:
//
//  v28 v2 660%0
//
#define AICF_WSO_FUELING_MSG             0x000006C1L

//
// v28 callsign of tanker
//
// v2 callsign of flight
//
// 661 <Disconnect>
// v28 v2 661
//
// MessageId: AICF_WSO_DISCONNECT_MSG
//
// MessageText:
//
//  v28 v2 661%0
//
#define AICF_WSO_DISCONNECT_MSG          0x000006C2L

//
// v25 callsign of flight
//
// 766 <acknowledged>
// v25 766
//
// MessageId: AICF_WSO_TANKER_ACKNOWLEDGED_MSG
//
// MessageText:
//
//  v25 766%0
//
#define AICF_WSO_TANKER_ACKNOWLEDGED_MSG 0x000006C3L

//
// v27 callsign of tanker
//
// v25 callsign of flight
//
// 662 <thanks for the gas>
// v27 v25 662
//
// MessageId: AICF_WSO_TANKER_THANKS_MSG
//
// MessageText:
//
//  v27 v25 662%0
//
#define AICF_WSO_TANKER_THANKS_MSG       0x000006C4L

//
// v28 Pilot callsign
//
// 301 <cleared precontact>
// v28 301
//
// MessageId: AICF_TANKER_PRECONTACT_MSG
//
// MessageText:
//
//  v28 301%0
//
#define AICF_TANKER_PRECONTACT_MSG       0x000006C5L

//
// v28 callsign plane tanking
//
// 302 <cleared on>
// v28 302
//
// MessageId: AICF_TANKER_ON_MSG
//
// MessageText:
//
//  v28 302%0
//
#define AICF_TANKER_ON_MSG               0x000006C6L

//
// 305 <Offload complete, disconnecting>
// 305
//
// MessageId: AICF_TANKER_DISCONNECT_MSG
//
// MessageText:
//
//  305%0
//
#define AICF_TANKER_DISCONNECT_MSG       0x000006C7L

//
// 306 <Have a nice day>
//
// v27 callsign of group
// 306 v27
//
// MessageId: AICF_TANKER_HANDAY_MSG
//
// MessageText:
//
//  306 v27%0
//
#define AICF_TANKER_HANDAY_MSG           0x000006C8L

//
// 307 <Good Hunting>
//
// v27 callsign of group
// 307 v27
//
// MessageId: AICF_TANKER_HUNTING_MSG
//
// MessageText:
//
//  307 v27%0
//
#define AICF_TANKER_HUNTING_MSG          0x000006C9L

//
// 303 <Stablize>
// 303
//
// MessageId: AICF_TANKER_STABLIZE
//
// MessageText:
//
//  303%0
//
#define AICF_TANKER_STABLIZE             0x000006CAL

//
// v27 callsign of lead plane tanking
//
// v1 callsign of tanker
//
// 300 <acknowledge, turning to >
//
// r4 bearing (360 in 10 deg inc) <speed 300>
//
// r5 <angels> altitude of tanker / 1000
// v27 v1 300 r4 r5
//
// MessageId: AICF_TANKER_HEADING_ALT
//
// MessageText:
//
//  v27 v1 300 r4 r5%0
//
#define AICF_TANKER_HEADING_ALT          0x000006CBL

//
// v27 callsign of lead plane tanking
//
// v1 callsign of tanker
//
// 300 <acknowledge, turning to >
//
// r4 bearing (360 in 10 deg inc) <speed 300>
// v27 v1 300 r4
//
// MessageId: AICF_TANKER_HEADING_NOALT
//
// MessageText:
//
//  v27 v1 300 r4%0
//
#define AICF_TANKER_HEADING_NOALT        0x000006CCL

//
// 304 <Your nose is hot, set systems to standby>
// 304
//
// MessageId: AICF_TANKER_CHECK_SWITCHES
//
// MessageText:
//
//  304%0
//
#define AICF_TANKER_CHECK_SWITCHES       0x000006CDL

//
// 516 <Slower>
// Not Translated
//
// MessageId: AICF_WSO_SLOWER
//
// MessageText:
//
//  516%0
//
#define AICF_WSO_SLOWER                  0x000006CEL

//
// 517 <Watch your closure>
// Not Translated
//
// MessageId: AICF_WSO_WATCH_CLOSURE
//
// MessageText:
//
//  517%0
//
#define AICF_WSO_WATCH_CLOSURE           0x000006CFL

//
// v30 callsign for tanker (intense)
//
// 663 <Breakaway, Breakaway, Breakaway>
// v30 663
//
// MessageId: AICF_WSO_BREAKAWAY
//
// MessageText:
//
//  v30 663%0
//
#define AICF_WSO_BREAKAWAY               0x000006D0L

//
// 519 <E-mis> selected
// Not Translated
//
// MessageId: AICF_WSO_EMIS_SELECTED
//
// MessageText:
//
//  519%0
//
#define AICF_WSO_EMIS_SELECTED           0x000006D1L

//
// 58 <Right>
// 58
//
// MessageId: AICF_RIGHT
//
// MessageText:
//
//  58%0
//
#define AICF_RIGHT                       0x000006D2L

//
// 57 <Left>
// 57
//
// MessageId: AICF_LEFT
//
// MessageText:
//
//  57%0
//
#define AICF_LEFT                        0x000006D3L

//
// 514 <Up>
// Not Translated
//
// MessageId: AICF_UP
//
// MessageText:
//
//  514%0
//
#define AICF_UP                          0x000006D4L

//
// 515 <Down>
// Not Translated
//
// MessageId: AICF_DOWN
//
// MessageText:
//
//  515%0
//
#define AICF_DOWN                        0x000006D5L

//
// v33 Distance remaining in ft, increments of 10.
// Not Translated
//
// MessageId: AICF_WSO_FT_DIST
//
// MessageText:
//
//  v33%0
//
#define AICF_WSO_FT_DIST                 0x000006D6L

//
// v10 Distance remaining in nms.
// Not Translated
//
// MessageId: AICF_WSO_MILES
//
// MessageText:
//
//  v10%0
//
#define AICF_WSO_MILES                   0x000006D7L

//
// v2 Pilot callsign
//
// 755 <copy, RTB>
// v2 755
//
// MessageId: AICF_RTB_1
//
// MessageText:
//
//  v2 755%0
//
#define AICF_RTB_1                       0x000006D8L

//
// v2 Pilot callsign
//
// 756 <acknowledged, returning to base.
// v2 756
//
// MessageId: AICF_RTB_2
//
// MessageText:
//
//  v2 756%0
//
#define AICF_RTB_2                       0x000006D9L

//
// v2 Pilot callsign
//
// 757 <is headed for home>
// v2 757
//
// MessageId: AICF_RTB_3
//
// MessageText:
//
//  v2 757%0
//
#define AICF_RTB_3                       0x000006DAL

//
// v35 Tower callsign
//
// v25 Flight callsign
//
// 677 <request takeoff clearance>
// v35 v25 677
//
// MessageId: AICF_WSO_REQUEST_TAKEOFF
//
// MessageText:
//
//  v35 v25 677%0
//
#define AICF_WSO_REQUEST_TAKEOFF         0x000006DBL

//
// v35 Tower callsign
//
// v25 Flight callsign
//
// 678 <request landing clearance>
// v35 v25 678
//
// MessageId: AICF_WSO_REQUEST_LANDING
//
// MessageText:
//
//  v35 v25 678%0
//
#define AICF_WSO_REQUEST_LANDING         0x000006DCL

//
// v25 Flight callsign
//
// 679 <run em up>
// v25 679
//
// MessageId: AICF_WSO_RUN_EM
//
// MessageText:
//
//  v25 679%0
//
#define AICF_WSO_RUN_EM                  0x000006DDL

//
// v27 Player plane callsign
//
// r3 winds string
//
// r4 ceiling string
//
// r5 visablitly
//
// 402 <cleared for takeoff, good luck>
// v27 r3 r4 r5 402
//
// MessageId: AICF_TOWER_CLEAR_TAKEOFF
//
// MessageText:
//
//  v27 r3 r4 r5 402%0
//
#define AICF_TOWER_CLEAR_TAKEOFF         0x000006DEL

//
// v27 Player callsign
//
// r3 winds string
//
// r4 ceiling string
//
// r5 visablitly
//
// 401 <cleared for takeoff, contact>
//
// r6 AWACS call sign <for airborne control>
// v27 r3 r4 r5 401
//
// MessageId: AICF_TOWER_TAKEOFF_CONTACT
//
// MessageText:
//
//  v27 r3 r4 r5 401%0
//
#define AICF_TOWER_TAKEOFF_CONTACT       0x000006DFL

//
// v27 Player callsign
//
// 403 <hold short for landing traffic>
// v27 403
//
// MessageId: AICF_TOWER_HOLD_SHORT
//
// MessageText:
//
//  v27 403%0
//
#define AICF_TOWER_HOLD_SHORT            0x000006E0L

//
// v27 Player callsign
//
// 404 <Taxi into position and hold, awaiting IFR release.>
// v27 404
//
// MessageId: AICF_TOWER_HOLD_IFR
//
// MessageText:
//
//  v27 404%0
//
#define AICF_TOWER_HOLD_IFR              0x000006E1L

//
// v27 Player callsign
//
// 428 <Cleared for landing>
// v27 428
//
// MessageId: AICF_TOWER_CLEAR_LAND
//
// MessageText:
//
//  v27 428%0
//
#define AICF_TOWER_CLEAR_LAND            0x000006E2L

//
// v27 Player callsign
//
// 429 <hold for traffic at>
//
// v40 1000s of feet <thousand>
// v27 429 v40
//
// MessageId: AICF_TOWER_HOLD_ALTITUDE
//
// MessageText:
//
//  v27 429 v40%0
//
#define AICF_TOWER_HOLD_ALTITUDE         0x000006E3L

//
// v27 Player callsign
//
// 431 <scramble, scramble, bogies inbound from the>
//
// v8 cardinal direction
// v27 431 v8
//
// MessageId: AICF_TOWER_SCRAMBLE
//
// MessageText:
//
//  v27 431 v8%0
//
#define AICF_TOWER_SCRAMBLE              0x000006E4L

//
// v27 Player callsign
//
// 430 <friendly traffic bearing>
//
// v7 bearing
//
// v10 range
//
// v40 altitude in thousands
// v27 430 v7 v10 v40
//
// MessageId: AICF_TOWER_FRIENDLY_TRAFFIC
//
// MessageText:
//
//  v27 430 v7 v10 v40%0
//
#define AICF_TOWER_FRIENDLY_TRAFFIC      0x000006E5L

//
// 300 <Picture clean>
// 300
//
// MessageId: AICF_AWACS_PICT_CLEAN
//
// MessageText:
//
//  300%0
//
#define AICF_AWACS_PICT_CLEAN            0x000006E6L

//
// 301 <Picture unchanged>
// 301
//
// MessageId: AICF_AWACS_PICT_UNCHANGED
//
// MessageText:
//
//  301%0
//
#define AICF_AWACS_PICT_UNCHANGED        0x000006E7L

//
// 302 <Picture same>
// 302
//
// MessageId: AICF_AWACS_PICT_SAME
//
// MessageText:
//
//  302%0
//
#define AICF_AWACS_PICT_SAME             0x000006E8L

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 303 <clean>
// v27 v1 303
//
// MessageId: AICF_AWACS_CLEAN
//
// MessageText:
//
//  v27 v1 303%0
//
#define AICF_AWACS_CLEAN                 0x000006E9L

//
// v1 AWACS callsign
//
// v38 speed (fast/slow mover)
//
// v20 size (group/single)
//
// v9 cardinal direction <bullseye> 
//
// v10 range (# miles)
//
// v11 altitude (high/med/low)
// Not Translated
//
// MessageId: AICF_AWACS_BULLSEYE_PICT
//
// MessageText:
//
//  v1 v38 v20 v9 v10 v11%0
//
#define AICF_AWACS_BULLSEYE_PICT         0x000006EAL

//
// v27 fighter callsign
//
// v1 AWACS callsign
//
// v38 speed (fast/slow mover)
//
// v20 size (group/single)
//
// 304 <bearing> 
//
// v7 bearing in degrees
//
// v10 range (# miles)
//
// v11 altitude (high/med/low)
// v27 v1  v38 v20 304 v7 v10 v11
//
// MessageId: AICF_AWACS_BEARING_PICT
//
// MessageText:
//
//  v27 v1  v38 v20 304 v7 v10 v11%0
//
#define AICF_AWACS_BEARING_PICT          0x000006EBL

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// v38 speed (fast/slow mover)
//
// v20 size (group/single)
//
// 304 <bearing>
//
// v7 bearing
//
// v10 range
//
// v11 altitude
//
// v36 aspect from player
// v27 v1 v38 v20 304 v7 v10 v11 v36
//
// MessageId: AICF_AWACS_BEARING
//
// MessageText:
//
//  v27 v1 v38 v20 304 v7 v10 v11 v36%0
//
#define AICF_AWACS_BEARING               0x000006ECL

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// v38 speed <fast/slow> <mover>
//
// 305 <threat bearing>
//
// v7 bearing
//
// v10 range
//
// v11 altitude
//
// v36 aspect
// v27 v1 v38 305 v7 v10 v11 v36
//
// MessageId: AICF_AWACS_THREAT_BEARING
//
// MessageText:
//
//  v27 v1 v38 305 v7 v10 v11 v36%0
//
#define AICF_AWACS_THREAT_BEARING        0x000006EDL

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 306 <friendly traffic bearing>
//
// v7 bearing
//
// v10 range
//
// v11 altitude
//
// v36 aspect
// v27 v1 306 v7 v10 v11 v36
//
// MessageId: AICF_AWACS_FRIENDLY_BEARING
//
// MessageText:
//
//  v27 v1 306 v7 v10 v11 v36%0
//
#define AICF_AWACS_FRIENDLY_BEARING      0x000006EEL

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 307 <be advised, new threats airborne at>
//
// 304 <bearing>
//
// v7 bearing
//
// v10 range
// v27 v1 307 304 v7 v10 v11 v36
//
// MessageId: AICF_AWACS_NEW_THREAT_BEARING
//
// MessageText:
//
//  v27 v1 307 304 v7 v10 v11 v36%0
//
#define AICF_AWACS_NEW_THREAT_BEARING    0x000006EFL

//
// v1 AWACS callsign
//
// 307 <be advised, new threats airborne at>
//
// v43 cardinal direction <bullseye> 
//
// v10 range
// v1 307 v43 v10
//
// MessageId: AICF_AWACS_NEW_THREAT_BULLSEYE
//
// MessageText:
//
//  v1 307 v43 v10%0
//
#define AICF_AWACS_NEW_THREAT_BULLSEYE   0x000006F0L

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 308 <merged>
// v27 v1 308
//
// MessageId: AICF_AWACS_MERGED
//
// MessageText:
//
//  v27 v1 308%0
//
#define AICF_AWACS_MERGED                0x000006F1L

//
// v27 Player callsign
//
// 309 <copy>
// v27 309
//
// MessageId: AICF_AWACS_COPY
//
// MessageText:
//
//  v27 309%0
//
#define AICF_AWACS_COPY                  0x000006F2L

//
// v27 Player callsign
//
// 310 <acknowledged>
// v27 310
//
// MessageId: AICF_AWACS_ACKNOWLEDGED
//
// MessageText:
//
//  v27 310%0
//
#define AICF_AWACS_ACKNOWLEDGED          0x000006F3L

//
// v27 Player callsign
//
// 511 <copy, SAR package enroute>
// v27 312
//
// MessageId: AICF_AWACS_SAR_ACK
//
// MessageText:
//
//  v27 312%0
//
#define AICF_AWACS_SAR_ACK               0x000006F4L

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 313 <negative>
// v27 v1 v313
//
// MessageId: AICF_AWACS_NEGATIVE
//
// MessageText:
//
//  v27 v1 v313%0
//
#define AICF_AWACS_NEGATIVE              0x000006F5L

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 314 <copy, vectoring fighters to your location.>
// v27 v1 314
//
// MessageId: AICF_AWACS_VECTORING_FIGHTERS
//
// MessageText:
//
//  v27 v1 314%0
//
#define AICF_AWACS_VECTORING_FIGHTERS    0x000006F6L

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 316 <negative, no assets available.>
// v27 v1 316
//
// MessageId: AICF_AWACS_NO_FIGHTERS
//
// MessageText:
//
//  v27 v1 316%0
//
#define AICF_AWACS_NO_FIGHTERS           0x000006F7L

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 317 <copy, Weasel support inbound>.
// v27 v1 317
//
// MessageId: AICF_AWACS_WEASELS
//
// MessageText:
//
//  v27 v1 317%0
//
#define AICF_AWACS_WEASELS               0x000006F8L

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 318 <negative, no Weasels avaiable.>
// v27 v1 316
//
// MessageId: AICF_AWACS_NO_WEASELS
//
// MessageText:
//
//  v27 v1 316%0
//
#define AICF_AWACS_NO_WEASELS            0x000006F9L

//
// 550 <Sequence points updated, new steering ready>
// Not Translated
//
// MessageId: AICF_WSO_NEW_STEERING_READY
//
// MessageText:
//
//  550%0
//
#define AICF_WSO_NEW_STEERING_READY      0x000006FAL

//
// 551 <FAC location marked in the system>
// Not Translated
//
// MessageId: AICF_WSO_FAC_MARKED
//
// MessageText:
//
//  551%0
//
#define AICF_WSO_FAC_MARKED              0x000006FBL

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 326 <We have priority tasking for you.  Standby for words.>
// v27 v1 326
//
// MessageId: AICF_AWACS_STANDBY
//
// MessageText:
//
//  v27 v1 326%0
//
#define AICF_AWACS_STANDBY               0x000006FCL

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 327 <Vector Immediately to new coordinates to support ground forces.>
// v27 v1 327
//
// MessageId: AICF_AWACS_SUPPORT_GROUND
//
// MessageText:
//
//  v27 v1 327%0
//
#define AICF_AWACS_SUPPORT_GROUND        0x000006FDL

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 328 <transmitting new coordinates on dolly.>
// v27 v1 328
//
// MessageId: AICF_AWACS_NEW_COORDINATES
//
// MessageText:
//
//  v27 v1 328%0
//
#define AICF_AWACS_NEW_COORDINATES       0x000006FEL

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 329 <upon arrival at new coordinates hold and orbit.  Ground FAC will contact you.>
// v27 v1 329
//
// MessageId: AICF_AWACS_GROUND_FAC
//
// MessageText:
//
//  v27 v1 329%0
//
#define AICF_AWACS_GROUND_FAC            0x000006FFL

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 200 <we are under attack by enemy troops, request immediate support.>
// Not Translated
//
// MessageId: AICF_GROUND_CONTACT_US_1
//
// MessageText:
//
//  v27 v34 200%0
//
#define AICF_GROUND_CONTACT_US_1         0x00000700L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 201 <we are under attack!>
// Not Translated
//
// MessageId: AICF_GROUND_CONTACT_US_2
//
// MessageText:
//
//  v27 v34 201%0
//
#define AICF_GROUND_CONTACT_US_2         0x00000701L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 202 <we are under attack, request assistance.>
// Not Translated
//
// MessageId: AICF_GROUND_CONTACT_US_3
//
// MessageText:
//
//  v27 v34 202%0
//
#define AICF_GROUND_CONTACT_US_3         0x00000702L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 200 <we think we've been discovered, request support.>
// Not Translated
//
// MessageId: AICF_GROUND_CONTACT_UK_1
//
// MessageText:
//
//  v27 v34 200%0
//
#define AICF_GROUND_CONTACT_UK_1         0x00000703L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 201 <we seen to have come under fire>
// Not Translated
//
// MessageId: AICF_GROUND_CONTACT_UK_2
//
// MessageText:
//
//  v27 v34 201%0
//
#define AICF_GROUND_CONTACT_UK_2         0x00000704L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 202 <we have been discovered, please come quickly.>
// Not Translated
//
// MessageId: AICF_GROUND_CONTACT_UK_3
//
// MessageText:
//
//  v27 v34 202%0
//
#define AICF_GROUND_CONTACT_UK_3         0x00000705L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 203 <request air strike on>
//
// r4 <enemy units> cardinal direction <of our position.>
// Not Translated
//
// MessageId: AICF_GROUND_NEED_HELP_1
//
// MessageText:
//
//  v27 v34 203 r4%0
//
#define AICF_GROUND_NEED_HELP_1          0x00000706L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 204 <requesting close air support against>
//
// r4 <enemy units> cardinal direction <of our position.>
// Not Translated
//
// MessageId: AICF_GROUND_NEED_HELP_2
//
// MessageText:
//
//  v27 v34 204 r4%0
//
#define AICF_GROUND_NEED_HELP_2          0x00000707L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 205 <we need air support on enemy units %3 of our position>
//
// r4 <enemy units> cardinal direction <of our position.>
// Not Translated
//
// MessageId: AICF_GROUND_NEED_HELP_3
//
// MessageText:
//
//  v27 v34 205 r4%0
//
#define AICF_GROUND_NEED_HELP_3          0x00000708L

//
// v27 Player callsign
//
// 333 <Check fire!  Check fire!  Your weapons are landing in our perimeter!>
// v27 333
//
// MessageId: AICF_GROUND_FRIENDLY_FIRE_US_1
//
// MessageText:
//
//  v27 333%0
//
#define AICF_GROUND_FRIENDLY_FIRE_US_1   0x00000709L

//
// v27 Player callsign
//
// 334 <Check fire!  There are friendlies in the area!>
// v27 334
//
// MessageId: AICF_GROUND_FRIENDLY_FIRE_US_2
//
// MessageText:
//
//  v27 334%0
//
#define AICF_GROUND_FRIENDLY_FIRE_US_2   0x0000070AL

//
// v27 Player callsign
//
// 335 <We're on the ground, watch your fire!>
// v27 335
//
// MessageId: AICF_GROUND_FRIENDLY_FIRE_US_3
//
// MessageText:
//
//  v27 335%0
//
#define AICF_GROUND_FRIENDLY_FIRE_US_3   0x0000070BL

//
// v27 Player callsign
//
// 333 <Please watch your aim.>
// v27 333
//
// MessageId: AICF_GROUND_FRIENDLY_FIRE_UK_1
//
// MessageText:
//
//  v27 333%0
//
#define AICF_GROUND_FRIENDLY_FIRE_UK_1   0x0000070CL

//
// v27 Player callsign
//
// 334 <Careful there blokeà you seem to be hitting our men!>
// v27 334
//
// MessageId: AICF_GROUND_FRIENDLY_FIRE_UK_2
//
// MessageText:
//
//  v27 334%0
//
#define AICF_GROUND_FRIENDLY_FIRE_UK_2   0x0000070DL

//
// v27 Player callsign
//
// 335 <We are on the same side, no?>
// v27 335
//
// MessageId: AICF_GROUND_FRIENDLY_FIRE_UK_3
//
// MessageText:
//
//  v27 335%0
//
#define AICF_GROUND_FRIENDLY_FIRE_UK_3   0x0000070EL

//
// v27 Player callsign
//
// 337 <You got them!  Great job!
// v27 337
//
// MessageId: AICF_GROUND_GOOD_US_1
//
// MessageText:
//
//  v27 337%0
//
#define AICF_GROUND_GOOD_US_1            0x0000070FL

//
// v27 Player callsign
//
// 338 <Good drop!>
// v27 338
//
// MessageId: AICF_GROUND_GOOD_US_2
//
// MessageText:
//
//  v27 338%0
//
#define AICF_GROUND_GOOD_US_2            0x00000710L

//
// v27 Player callsign
//
// 339 <Enemy is withdrawing, thanks a lot.>
// v27 339
//
// MessageId: AICF_GROUND_GOOD_US_3
//
// MessageText:
//
//  v27 339%0
//
#define AICF_GROUND_GOOD_US_3            0x00000711L

//
// v27 Player callsign
//
// 337 <Jolly good show mates!>
// v27 337
//
// MessageId: AICF_GROUND_GOOD_UK_1
//
// MessageText:
//
//  v27 337%0
//
#define AICF_GROUND_GOOD_UK_1            0x00000712L

//
// v27 Player callsign
//
// 338 <Good work lads!>
// v27 338
//
// MessageId: AICF_GROUND_GOOD_UK_2
//
// MessageText:
//
//  v27 338%0
//
#define AICF_GROUND_GOOD_UK_2            0x00000713L

//
// v27 Player callsign
//
// 339 <That's got the buggers on the run!>
// v27 339
//
// MessageId: AICF_GROUND_GOOD_UK_3
//
// MessageText:
//
//  v27 339%0
//
#define AICF_GROUND_GOOD_UK_3            0x00000714L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 229 <We can't hold out any longerà>
// Not Translated
//
// MessageId: AICF_GROUND_DEAD_US_1
//
// MessageText:
//
//  v27 v34 229%0
//
#define AICF_GROUND_DEAD_US_1            0x00000715L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 230 <Too late!>
// Not Translated
//
// MessageId: AICF_GROUND_DEAD_US_2
//
// MessageText:
//
//  v27 v34 230%0
//
#define AICF_GROUND_DEAD_US_2            0x00000716L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 229 <Sorry mates, we're done forà>
// Not Translated
//
// MessageId: AICF_GROUND_DEAD_UK_1
//
// MessageText:
//
//  v27 v34 229%0
//
#define AICF_GROUND_DEAD_UK_1            0x00000717L

//
// v27 Player callsign
//
// v34 Ground callsign
//
// 230 <We've had it ladsà  God save the queen!>
// Not Translated
//
// MessageId: AICF_GROUND_DEAD_UK_2
//
// MessageText:
//
//  v27 v34 230%0
//
#define AICF_GROUND_DEAD_UK_2            0x00000718L

//
// v27 Player callsign
//
// 340 <Be advised enemy forces have withdrawn.>
// v27 340
//
// MessageId: AICF_GROUND_ENEMY_GONE_US
//
// MessageText:
//
//  v27 340%0
//
#define AICF_GROUND_ENEMY_GONE_US        0x00000719L

//
// v27 Player callsign
//
// 340 <The enemy seems to have run away.>
// v27 340
//
// MessageId: AICF_GROUND_ENEMY_GONE_UK
//
// MessageText:
//
//  v27 340%0
//
#define AICF_GROUND_ENEMY_GONE_UK        0x0000071AL

//
// v27 AWACS callsign
//
// v1 SEAD callsign
//
// 584 <on station>
// v27 v1 584
//
// MessageId: AICF_SEAD_ON_STATION
//
// MessageText:
//
//  v27 v1 584%0
//
#define AICF_SEAD_ON_STATION             0x0000071BL

//
// v27 Player callsign
//
// v1 fighter callsign
//
// 680 <we have you in sight, forming up.>
// v27 v1 680
//
// MessageId: AICF_CAP_FORM_PLAYER
//
// MessageText:
//
//  v27 v1 680%0
//
#define AICF_CAP_FORM_PLAYER             0x0000071CL

//
// v27 Player callsign
//
// v1 fighter callsign
//
// 681 <on our way.>
// v27 v1 681
//
// MessageId: AICF_CAP_ON_WAY
//
// MessageText:
//
//  v27 v1 681%0
//
#define AICF_CAP_ON_WAY                  0x0000071DL

//
// v27 Player callsign
//
// v1 fighter callsign
//
// 682 <we're with you.>
// v27 v1 682
//
// MessageId: AICF_CAP_WITH_YOU
//
// MessageText:
//
//  v27 v1 682%0
//
#define AICF_CAP_WITH_YOU                0x0000071EL

//
// v27 Player callsign
//
// v1 SEAD callsign
//
// 680 <we have you in sight, forming up.>
// v27 v1 680
//
// MessageId: AICF_SEAD_FORM_PLAYER
//
// MessageText:
//
//  v27 v1 680%0
//
#define AICF_SEAD_FORM_PLAYER            0x0000071FL

//
// v27 Player callsign
//
// v1 SEAD callsign
//
// 681 <on our way.>
// v27 v1 681
//
// MessageId: AICF_SEAD_ON_WAY
//
// MessageText:
//
//  v27 v1 681%0
//
#define AICF_SEAD_ON_WAY                 0x00000720L

//
// v27 Player callsign
//
// v1 SEAD callsign
//
// 682 <we're with you.>
// v27 v1 682
//
// MessageId: AICF_SEAD_WITH_YOU
//
// MessageText:
//
//  v27 v1 682%0
//
#define AICF_SEAD_WITH_YOU               0x00000721L

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 300 <we are inbound at this time.>
// v27 v1 300
//
// MessageId: AICF_SAR_INBOUND
//
// MessageText:
//
//  v27 v1 300%0
//
#define AICF_SAR_INBOUND                 0x00000722L

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 301 <on our way.>
// v27 v1 301
//
// MessageId: AICF_SAR_ON_WAY
//
// MessageText:
//
//  v27 v1 301%0
//
#define AICF_SAR_ON_WAY                  0x00000723L

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 302 <scambling to designated coordinates.>
// v27 v1 302
//
// MessageId: AICF_SAR_SCRAMBLE
//
// MessageText:
//
//  v27 v1 302%0
//
#define AICF_SAR_SCRAMBLE                0x00000724L

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 303 <we are moving in to LZ at this time, over.>
// v27 v1 303
//
// MessageId: AICF_SAR_MOVING_IN_LZ
//
// MessageText:
//
//  v27 v1 303%0
//
#define AICF_SAR_MOVING_IN_LZ            0x00000725L

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 304 <be advised, we are moving in, over.>
// v27 v1 304
//
// MessageId: AICF_SAR_ADVISED_MOVING_IN
//
// MessageText:
//
//  v27 v1 304%0
//
#define AICF_SAR_ADVISED_MOVING_IN       0x00000726L

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 305 <we are now approaching LZ, over.>
// v27 v1 305
//
// MessageId: AICF_SAR_APPROACHING_LZ
//
// MessageText:
//
//  v27 v1 305%0
//
#define AICF_SAR_APPROACHING_LZ          0x00000727L

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 306 <we're down at LZ, over.>
// v27 v1 306
//
// MessageId: AICF_SAR_DOWN_AT_LZ
//
// MessageText:
//
//  v27 v1 306%0
//
#define AICF_SAR_DOWN_AT_LZ              0x00000728L

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 307 <we are now at LZ, over.>
// v27 v1 307
//
// MessageId: AICF_SAR_NOW_AT_LZ
//
// MessageText:
//
//  v27 v1 307%0
//
#define AICF_SAR_NOW_AT_LZ               0x00000729L

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 308 <touching down at LZ now, over.>
// v27 v1 308
//
// MessageId: AICF_SAR_TOUCHING_LZ
//
// MessageText:
//
//  v27 v1 308%0
//
#define AICF_SAR_TOUCHING_LZ             0x0000072AL

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 309 <pickup successful, returning to base.>
// v27 v1 309
//
// MessageId: AICF_SAR_PICKUP_OK
//
// MessageText:
//
//  v27 v1 309%0
//
#define AICF_SAR_PICKUP_OK               0x0000072BL

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 310 <we got em, heading home.>
// v27 v1 310
//
// MessageId: AICF_SAR_GOT_EM
//
// MessageText:
//
//  v27 v1 310%0
//
#define AICF_SAR_GOT_EM                  0x0000072CL

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 311 <be advised, we are unable to locate the downed crew.>
// v27 v1 311
//
// MessageId: AICF_SAR_NO_FIND
//
// MessageText:
//
//  v27 v1 311%0
//
#define AICF_SAR_NO_FIND                 0x0000072DL

//
// v27 AWACS callsign
//
// v1 SAR callsign
//
// 312 <no friendlies at LZ, returning to base.>
// v27 v1 312
//
// MessageId: AICF_SAR_NO_FRIENDLIES
//
// MessageText:
//
//  v27 v1 312%0
//
#define AICF_SAR_NO_FRIENDLIES           0x0000072EL

//
// 331 <Splash our bandit!>
// Not Translated
//
// MessageId: AICF_WSO_SPLASH_BANDIT
//
// MessageText:
//
//  331%0
//
#define AICF_WSO_SPLASH_BANDIT           0x0000072FL

//
// 332 <That's a kill!>
// Not Translated
//
// MessageId: AICF_WSO_KILL
//
// MessageText:
//
//  332%0
//
#define AICF_WSO_KILL                    0x00000730L

//
// 333 <Splash one helo!>
// Not Translated
//
// MessageId: AICF_WSO_SPLASH_HELO
//
// MessageText:
//
//  333%0
//
#define AICF_WSO_SPLASH_HELO             0x00000731L

//
// 334 <We got him!>
// Not Translated
//
// MessageId: AICF_WSO_GOT_HIM
//
// MessageText:
//
//  334%0
//
#define AICF_WSO_GOT_HIM                 0x00000732L

//
// 335 <That guy is history>
// Not Translated
//
// MessageId: AICF_WSO_HISTORY
//
// MessageText:
//
//  335%0
//
#define AICF_WSO_HISTORY                 0x00000733L

//
// 336 <Spash!>
// Not Translated
//
// MessageId: AICF_WSO_SPLASH
//
// MessageText:
//
//  336%0
//
#define AICF_WSO_SPLASH                  0x00000734L

//
// v5 Wingman callsign
//
// 702 <Splash my bandit!>
// v5 702
//
// MessageId: AICF_WING_SPLASH_1
//
// MessageText:
//
//  v5 702%0
//
#define AICF_WING_SPLASH_1               0x00000735L

//
// v5 Wingman callsign
//
// 703 <splash>
//
// r2 number
// v5 703 r2
//
// MessageId: AICF_WING_SPLASH_2
//
// MessageText:
//
//  v5 703 r2%0
//
#define AICF_WING_SPLASH_2               0x00000736L

//
// v5 Wingman callsign
//
// 704<bandit destoryed!>
// v5 704
//
// MessageId: AICF_WING_SPLASH_3
//
// MessageText:
//
//  v5 704%0
//
#define AICF_WING_SPLASH_3               0x00000737L

//
// 322 <Good kill lead!
// Not Translated
//
// MessageId: AICF_PLAYER_KILL_1
//
// MessageText:
//
//  322%0
//
#define AICF_PLAYER_KILL_1               0x00000738L

//
// v5 Wingman callsign
//
// 705 <confirmed, that's a kill!>
// v5 705
//
// MessageId: AICF_PLAYER_KILL_2
//
// MessageText:
//
//  v5 705%0
//
#define AICF_PLAYER_KILL_2               0x00000739L

//
// 706 <Good kill>
//
// v31 Plane callsign with number intense.
// 706 v31
//
// MessageId: AICF_PLAYER_KILL_3
//
// MessageText:
//
//  706 v31%0
//
#define AICF_PLAYER_KILL_3               0x0000073AL

//
// 563 <What the hell are you doing?>
// Not Translated
//
// MessageId: AICF_WSO_WHAT_THE_HELL
//
// MessageText:
//
//  563%0
//
#define AICF_WSO_WHAT_THE_HELL           0x0000073BL

//
// 564 <Oh, that was smart!>
// Not Translated
//
// MessageId: AICF_WSO_THAT_WAS_SMART
//
// MessageText:
//
//  564%0
//
#define AICF_WSO_THAT_WAS_SMART          0x0000073CL

//
// 565 <What the hell Is wrong with you!>
// Not Translated
//
// MessageId: AICF_WSO_WHAT_HELL_WRONG
//
// MessageText:
//
//  565%0
//
#define AICF_WSO_WHAT_HELL_WRONG         0x0000073DL

//
// 566 <That was STUPID!>
// Not Translated
//
// MessageId: AICF_WSO_THAT_WAS_STUPID
//
// MessageText:
//
//  566%0
//
#define AICF_WSO_THAT_WAS_STUPID         0x0000073EL

//
// 247 <We're taking triple-A hits>
// Not Translated
//
// MessageId: AICF_WSO_AAA_HITS_1
//
// MessageText:
//
//  247%0
//
#define AICF_WSO_AAA_HITS_1              0x0000073FL

//
// 248 <We've been hit!>
// Not Translated
//
// MessageId: AICF_WSO_AAA_HITS_2
//
// MessageText:
//
//  248%0
//
#define AICF_WSO_AAA_HITS_2              0x00000740L

//
// 249 <We're taking heavy triple-A fire!>
// Not Translated
//
// MessageId: AICF_WSO_AAA_HITS_3
//
// MessageText:
//
//  249%0
//
#define AICF_WSO_AAA_HITS_3              0x00000741L

//
// 250 <Get above this stuff!>
// Not Translated
//
// MessageId: AICF_WSO_AAA_HITS_4
//
// MessageText:
//
//  250%0
//
#define AICF_WSO_AAA_HITS_4              0x00000742L

//
// 251 <We're hit!>
// Not Translated
//
// MessageId: AICF_WSO_CANNON_HIT_1
//
// MessageText:
//
//  251%0
//
#define AICF_WSO_CANNON_HIT_1            0x00000743L

//
// 252 <We're taking fire!>
// Not Translated
//
// MessageId: AICF_WSO_CANNON_HIT_2
//
// MessageText:
//
//  252%0
//
#define AICF_WSO_CANNON_HIT_2            0x00000744L

//
// 253 <We're taking cannon fire!>
// Not Translated
//
// MessageId: AICF_WSO_CANNON_HIT_3
//
// MessageText:
//
//  253%0
//
#define AICF_WSO_CANNON_HIT_3            0x00000745L

//
// 254 <That missile just hit us!>
// Not Translated
//
// MessageId: AICF_WSO_MISSILE_HIT_1
//
// MessageText:
//
//  254%0
//
#define AICF_WSO_MISSILE_HIT_1           0x00000746L

//
// 255 <I think we just took a missile hit!>
// Not Translated
//
// MessageId: AICF_WSO_MISSILE_HIT_2
//
// MessageText:
//
//  255%0
//
#define AICF_WSO_MISSILE_HIT_2           0x00000747L

//
// 256 <We've been hit!>
// Not Translated
//
// MessageId: AICF_WSO_MISSILE_HIT_3
//
// MessageText:
//
//  256%0
//
#define AICF_WSO_MISSILE_HIT_3           0x00000748L

//
// 257 <Uh oh! That hurt!>
// Not Translated
//
// MessageId: AICF_WSO_MISSILE_HIT_4
//
// MessageText:
//
//  257%0
//
#define AICF_WSO_MISSILE_HIT_4           0x00000749L

//
// v5 Wingman callsign
//
// 543 <has taken hits from the ground!>
// v5 543
//
// MessageId: AICF_WING_HITS_FROM_GROUND
//
// MessageText:
//
//  v5 543%0
//
#define AICF_WING_HITS_FROM_GROUND       0x0000074AL

//
// v5 Wingman callsign
//
// 544 <we've been hit!>
// v5 544
//
// MessageId: AICF_WING_HIT
//
// MessageText:
//
//  v5 544%0
//
#define AICF_WING_HIT                    0x0000074BL

//
// v5 Wingman callsign
//
// 545 <we're hurt, assessing damage now!>
// v5 545
//
// MessageId: AICF_WING_HIT_HURT
//
// MessageText:
//
//  v5 545%0
//
#define AICF_WING_HIT_HURT               0x0000074CL

//
// v5 Wingman callsign
//
// 546 <we took some major hits!>
// v5 546
//
// MessageId: AICF_WING_HIT_MAJOR
//
// MessageText:
//
//  v5 546%0
//
#define AICF_WING_HIT_MAJOR              0x0000074DL

//
// v5 Wingman callsign
//
// 547 <ejecting now!>
// v5 547
//
// MessageId: AICF_WING_EJECTING
//
// MessageText:
//
//  v5 547%0
//
#define AICF_WING_EJECTING               0x0000074EL

//
// v31 player/wingman callsign intense
//
// 556 <you're on fire! Eject! Eject!
// v31 556
//
// MessageId: AICF_WING_EJECT_CALL_1
//
// MessageText:
//
//  v31 556%0
//
#define AICF_WING_EJECT_CALL_1           0x0000074FL

//
// v31 player/wingman callsign intense
//
// 557 <Eject!  Eject!
// v31 557
//
// MessageId: AICF_WING_EJECT_CALL_2
//
// MessageText:
//
//  v31 557%0
//
#define AICF_WING_EJECT_CALL_2           0x00000750L

//
// v31 player/wingman callsign intense
//
// 558 <get out of there!  Eject!>
// v31 558
//
// MessageId: AICF_WING_EJECT_CALL_3
//
// MessageText:
//
//  v31 558%0
//
#define AICF_WING_EJECT_CALL_3           0x00000751L

//
// v2 player/wingman callsign
//
// 692 <Fox 1!>
// v2 692
//
// MessageId: AICF_FOX_1
//
// MessageText:
//
//  v2 692%0
//
#define AICF_FOX_1                       0x00000752L

//
// v2 player/wingman callsign
//
// 693 <Fox 2!>
// v2 693
//
// MessageId: AICF_FOX_2
//
// MessageText:
//
//  v2 693%0
//
#define AICF_FOX_2                       0x00000753L

//
// v2 player/wingman callsign
//
// 694 <Fox 3!>
// v2 694
//
// MessageId: AICF_FOX_3
//
// MessageText:
//
//  v2 694%0
//
#define AICF_FOX_3                       0x00000754L

//
// v2 player/wingman callsign
//
// 695 <Guns!>
// v2 695
//
// MessageId: AICF_GUNS
//
// MessageText:
//
//  v2 695%0
//
#define AICF_GUNS                        0x00000755L

//
// v5 player/wingman callsign intense
//
// 341 <Fox 1!> intense
// Not Translated
//
// MessageId: AICF_INT_FOX_1
//
// MessageText:
//
//  v2 341%0
//
#define AICF_INT_FOX_1                   0x00000756L

//
// v5 player/wingman callsign intense
//
// 338 <Fox 2!> intense
// Not Translated
//
// MessageId: AICF_INT_FOX_2
//
// MessageText:
//
//  v2 342%0
//
#define AICF_INT_FOX_2                   0x00000757L

//
// v5 player/wingman callsign intense
//
// 343 <Fox 3!> intense
// Not Translated
//
// MessageId: AICF_INT_FOX_3
//
// MessageText:
//
//  v2 343%0
//
#define AICF_INT_FOX_3                   0x00000758L

//
// v5 player/wingman callsign intense
//
// 344 <Guns!> intense
// Not Translated
//
// MessageId: AICF_INT_GUNS
//
// MessageText:
//
//  v2 344%0
//
#define AICF_INT_GUNS                    0x00000759L

//
// 231 <He's firing cannon - Jink now!>
// Not Translated
//
// MessageId: AICF_ENEMY_GUNS_JINK
//
// MessageText:
//
//  231%0
//
#define AICF_ENEMY_GUNS_JINK             0x0000075AL

//
// 307 <Bombs are gone.>
// Not Translated
//
// MessageId: AICF_WSO_BOMBS_GONE
//
// MessageText:
//
//  307%0
//
#define AICF_WSO_BOMBS_GONE              0x0000075BL

//
// 308 <Release>
// Not Translated
//
// MessageId: AICF_WSO_RELEASE
//
// MessageText:
//
//  308%0
//
#define AICF_WSO_RELEASE                 0x0000075CL

//
// 309 <Bombs are gone, good release.>
// Not Translated
//
// MessageId: AICF_WSO_GOOD_RELEASE
//
// MessageText:
//
//  309%0
//
#define AICF_WSO_GOOD_RELEASE            0x0000075DL

//
// 310 <Pickle>
// Not Translated
//
// MessageId: AICF_WSO_PICKLE
//
// MessageText:
//
//  310%0
//
#define AICF_WSO_PICKLE                  0x0000075EL

//
// v19 miles <miles to target.>
// Not Translated
//
// MessageId: AICF_WSO_TARGET_DIST
//
// MessageText:
//
//  v19%0
//
#define AICF_WSO_TARGET_DIST             0x0000075FL

//
// v12 seconds <to impact>
// Not Translated
//
// MessageId: AICF_WSO_TIME_TO_IMPACT
//
// MessageText:
//
//  v12%0
//
#define AICF_WSO_TIME_TO_IMPACT          0x00000760L

//
// v17 seconds <until pull>
// Not Translated
//
// MessageId: AICF_WSO_PULL_TIME
//
// MessageText:
//
//  v17%0
//
#define AICF_WSO_PULL_TIME               0x00000761L

//
// 352 <Pull now!>
// Not Translated
//
// MessageId: AICF_WSO_PULL_NOW
//
// MessageText:
//
//  352%0
//
#define AICF_WSO_PULL_NOW                0x00000762L

//
// v16 seconds <to release>
// Not Translated
//
// MessageId: AICF_WSO_RELEASE_TIME
//
// MessageText:
//
//  v16%0
//
#define AICF_WSO_RELEASE_TIME            0x00000763L

//
// 299 <You're sloppy, watch your steering.>
// Not Translated
//
// MessageId: AICF_WSO_WATCH_STEERING
//
// MessageText:
//
//  299%0
//
#define AICF_WSO_WATCH_STEERING          0x00000764L

//
// 300 <Take your steering.>
// Not Translated
//
// MessageId: AICF_WSO_TAKE_STEERING
//
// MessageText:
//
//  300%0
//
#define AICF_WSO_TAKE_STEERING           0x00000765L

//
// 301 <Center your steering.>
// Not Translated
//
// MessageId: AICF_WSO_CENTER_STEERING
//
// MessageText:
//
//  301%0
//
#define AICF_WSO_CENTER_STEERING         0x00000766L

//
// 302 <Looking good>
// Not Translated
//
// MessageId: AICF_WSO_LOOKING_GOOD
//
// MessageText:
//
//  302%0
//
#define AICF_WSO_LOOKING_GOOD            0x00000767L

//
// v18 minutes <minutes to nav point.>
// Not Translated
//
// MessageId: AICF_WSO_MINUTES_TO_NAV
//
// MessageText:
//
//  v18%0
//
#define AICF_WSO_MINUTES_TO_NAV          0x00000768L

//
// v27 AWACS callsign
//
// r3 wignman callwign
//
// 654 <has ejected, request immediate SAR scramble.>
// v27 r3 654
//
// MessageId: AICF_WSO_SAR_CALL
//
// MessageText:
//
//  v27 r3 654%0
//
#define AICF_WSO_SAR_CALL                0x00000769L

//
// 764 <Any allied aircraft, any allied aircraft, this is>
//
// v5 Pilot callsign intense
// 764 v5
//
// MessageId: AICF_SHOT_DOWN_1
//
// MessageText:
//
//  764 v5%0
//
#define AICF_SHOT_DOWN_1                 0x0000076AL

//
// v31 Contact Pilot callsign intense
//
// 765 <do you read me?>
// v31 765
//
// MessageId: AICF_SHOT_DOWN_READ_ME
//
// MessageText:
//
//  v31 765%0
//
#define AICF_SHOT_DOWN_READ_ME           0x0000076BL

//
// v5 Pilot callsign intense
//
// 766 <acknowledged>
// v5 766
//
// MessageId: AICF_SHOT_DOWN_ACK
//
// MessageText:
//
//  v5 766%0
//
#define AICF_SHOT_DOWN_ACK               0x0000076CL

//
// v5 Pilot callsign intense
//
// 767 <enemy near! Hiding!
// v5 767
//
// MessageId: AICF_SHOT_DOWN_HIDING_US
//
// MessageText:
//
//  v5 767%0
//
#define AICF_SHOT_DOWN_HIDING_US         0x0000076DL

//
// v5 Pilot callsign intense
//
// 768 <understood>
// v5 768
//
// MessageId: AICF_SHOT_DOWN_UNDERSTOOD
//
// MessageText:
//
//  v5 768%0
//
#define AICF_SHOT_DOWN_UNDERSTOOD        0x0000076EL

//
// v5 Pilot callsign intense
//
// 769 <I am down safely!>
// v5 769
//
// MessageId: AICF_SHOT_DOWN_SAFE
//
// MessageText:
//
//  v5 769%0
//
#define AICF_SHOT_DOWN_SAFE              0x0000076FL

//
// v5 Pilot callsign intense
//
// 770 <I'm pretty beat up, send help!>
// v5 770
//
// MessageId: AICF_SHOT_DOWN_BEAT_UP_US
//
// MessageText:
//
//  v5 770%0
//
#define AICF_SHOT_DOWN_BEAT_UP_US        0x00000770L

//
// v28 wing callsign
//
// v1 Player callsign
//
// 655 <we hear you, help is on the way>
// v28 v1 655
//
// MessageId: AICF_WSO_HELP_ON_WAY
//
// MessageText:
//
//  v28 v1 655%0
//
#define AICF_WSO_HELP_ON_WAY             0x00000771L

//
// v28 wing callsign
//
// v1 Player callsign
//
// 656 <copy, hang in there.>
// v28 v1 656
//
// MessageId: AICF_WSO_HANG_IN_THERE
//
// MessageText:
//
//  v28 v1 656%0
//
#define AICF_WSO_HANG_IN_THERE           0x00000772L

//
// v35 Ground object callsign
//
// v1 Player callsign
//
// 656 <copy, hang in there.>
// v35 v1 656
//
// MessageId: AICF_WSO_GRND_HANG_IN_THERE
//
// MessageText:
//
//  v35 v1 656%0
//
#define AICF_WSO_GRND_HANG_IN_THERE      0x00000773L

//
// v28 wing callsign
//
// v1 Player callsign
//
// 657 <Sandy inbound>
// v28 v1 657
//
// MessageId: AICF_WSO_SANDY_INBOUND
//
// MessageText:
//
//  v28 v1 657%0
//
#define AICF_WSO_SANDY_INBOUND           0x00000774L

//
// v28 wing callsign
//
// 767 <acknowledged>
// v28 767
//
// MessageId: AICF_WSO_ACK
//
// MessageText:
//
//  v28 767%0
//
#define AICF_WSO_ACK                     0x00000775L

//
// v35 Ground object callsign
//
// 767 <acknowledged>
// v35 767
//
// MessageId: AICF_WSO_GRND_ACK
//
// MessageText:
//
//  v35 767%0
//
#define AICF_WSO_GRND_ACK                0x00000776L

//
// 519 <Incomming missile,>
//
// v37 Clock position
// 519 v37
//
// MessageId: AICF_INCOMING_MISSILE
//
// MessageText:
//
//  519 v37%0
//
#define AICF_INCOMING_MISSILE            0x00000777L

//
// 520 <Missile inbound,>
//
// v37 Clock position
// 520 v37
//
// MessageId: AICF_MISSILE_INBOUND
//
// MessageText:
//
//  520 v37%0
//
#define AICF_MISSILE_INBOUND             0x00000778L

//
// 521 <Heads up! Missile Launch, >
//
// v37 Clock position
// 521 v37
//
// MessageId: AICF_HEADS_UP_MISSILE
//
// MessageText:
//
//  521 v37%0
//
#define AICF_HEADS_UP_MISSILE            0x00000779L

//
// v6 Wingman number
//
// 568 <Missile Launch>
//
// v37 Clock position
// v6 568 v37
//
// MessageId: AICF_WING_RADAR_DETECT_6
//
// MessageText:
//
//  v6 568 v37%0
//
#define AICF_WING_RADAR_DETECT_6         0x0000077AL

//
// v3 Wingman number
//
// r2 radar missile string
//
// r3 heat missile string
//
// r4 harm missile string
//
// r5 AG Missile string
//
// r6 Guided bomb string
//
// r7 Dumb bomb string
//
// r8 Cluster bomb string
//
// r9 Pod string
//
// 713 <and gun>
// v3 r2 r3 r4 r5 r6 r7 r8 r9 713
//
// MessageId: AICF_WEAPONS_GUN
//
// MessageText:
//
//  v3 r2 r3 r4 r5 r6 r7 r8 r9 713%0
//
#define AICF_WEAPONS_GUN                 0x0000077BL

//
// v3  Wingman number
//
// r2 radar missile string
//
// r3 heat missile string
//
// r4 harm missile string
//
// r5 AG Missile string
//
// r6 Guided bomb string
//
// r7 Dumb bomb string
//
// r8 Cluster bomb string
//
// r9 Pod string
//
// 714 <and zero gun>
// v3 r2 r3 r4 r5 r6 r7 r8 r9 714
//
// MessageId: AICF_WEAPONS_NO_GUN
//
// MessageText:
//
//  v3 r2 r3 r4 r5 r6 r7 r8 r9 714%0
//
#define AICF_WEAPONS_NO_GUN              0x0000077CL

//
// 241 <Break Right!>
// Not Translated
//
// MessageId: AICF_WSO_BREAK_RIGHT
//
// MessageText:
//
//  241%0
//
#define AICF_WSO_BREAK_RIGHT             0x0000077DL

//
// 242 <Break Left!>
// Not Translated
//
// MessageId: AICF_WSO_BREAK_LEFT
//
// MessageText:
//
//  242%0
//
#define AICF_WSO_BREAK_LEFT              0x0000077EL

//
// 243 <Chaff!  Chaff!>
// Not Translated
//
// MessageId: AICF_WSO_CHAFF
//
// MessageText:
//
//  243%0
//
#define AICF_WSO_CHAFF                   0x0000077FL

//
// 244 <Flare!  Flare!
// Not Translated
//
// MessageId: AICF_WSO_FLARE
//
// MessageText:
//
//  244%0
//
#define AICF_WSO_FLARE                   0x00000780L

//
// v6 Wingman callsign
//
// 223 <is engaged defensive ATOLL>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_ATOLL
//
// MessageText:
//
//  v6 223%0
//
#define AICF_WING_DEFENSIVE_ATOLL        0x00000781L

//
// v6 Wingman callsign
//
// 224 <is engaged defensive APEX>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_APEX
//
// MessageText:
//
//  v6 224%0
//
#define AICF_WING_DEFENSIVE_APEX         0x00000782L

//
// v6 Wingman callsign
//
// 518 <is engaged defensive ARCHER>
// v6 518
//
// MessageId: AICF_WING_DEFENSIVE_ARCHER
//
// MessageText:
//
//  v6 518%0
//
#define AICF_WING_DEFENSIVE_ARCHER       0x00000783L

//
// 308 <We're hit, going it!>
// 308
//
// MessageId: AICF_TANKER_HIT_GOING_IN
//
// MessageText:
//
//  308%0
//
#define AICF_TANKER_HIT_GOING_IN         0x00000784L

//
// 309 <We're going downà ahhhà!>
// 309
//
// MessageId: AICF_TANKER_GOING_DOWN
//
// MessageText:
//
//  309%0
//
#define AICF_TANKER_GOING_DOWN           0x00000785L

//
// 310 <I can't control it! Bail out, bail out now!>
// 310
//
// MessageId: AICF_TANKER_BAIL_OUT
//
// MessageText:
//
//  310%0
//
#define AICF_TANKER_BAIL_OUT             0x00000786L

//
// 311 <She's not respondingà abandon ship!>
// 311
//
// MessageId: AICF_TANKER_ABANDON_SHIP
//
// MessageText:
//
//  311%0
//
#define AICF_TANKER_ABANDON_SHIP         0x00000787L

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 313 <Launch!  Launch!>
// v27 v4 313
//
// MessageId: AICF_SAR_LAUNCH_LAUNCH
//
// MessageText:
//
//  v27 v4 313%0
//
#define AICF_SAR_LAUNCH_LAUNCH           0x00000788L

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 314 <Missile Launch!>
// v27 v4 314
//
// MessageId: AICF_SAR_MISSILE_LAUNCH
//
// MessageText:
//
//  v27 v4 314%0
//
#define AICF_SAR_MISSILE_LAUNCH          0x00000789L

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 315 <SAM!  SAM!>
// v27 v4 315
//
// MessageId: AICF_SAR_SAM_SAM
//
// MessageText:
//
//  v27 v4 315%0
//
#define AICF_SAR_SAM_SAM                 0x0000078AL

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 316 <SAM Launch!>
// v27 v4 316
//
// MessageId: AICF_SAR_SAM_LAUNCH
//
// MessageText:
//
//  v27 v4 316%0
//
#define AICF_SAR_SAM_LAUNCH              0x0000078BL

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 317 <We're taking Triple-A fire!>
// v27 v4 317
//
// MessageId: AICF_SAR_AAA_FIRE
//
// MessageText:
//
//  v27 v4 317%0
//
#define AICF_SAR_AAA_FIRE                0x0000078CL

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 318 <encountering heavy Triple-A!>
// v27 v4 318
//
// MessageId: AICF_SAR_HEAVY_AAA
//
// MessageText:
//
//  v27 v4 318%0
//
#define AICF_SAR_HEAVY_AAA               0x0000078DL

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 319 <we're taking fire!>
// v27 v4 319
//
// MessageId: AICF_SAR_WE_TAKING_FIRE
//
// MessageText:
//
//  v27 v4 319%0
//
#define AICF_SAR_WE_TAKING_FIRE          0x0000078EL

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 320 <taking fire!>
// v27 v4 320
//
// MessageId: AICF_SAR_TAKING_FIRE
//
// MessageText:
//
//  v27 v4 320%0
//
#define AICF_SAR_TAKING_FIRE             0x0000078FL

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 321 <we're hit!>
// v27 v4 321
//
// MessageId: AICF_SAR_WE_ARE_HIT
//
// MessageText:
//
//  v27 v4 321%0
//
#define AICF_SAR_WE_ARE_HIT              0x00000790L

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 322 <we are taking heavy damage!>
// v27 v4 322
//
// MessageId: AICF_SAR_HEAVY_DAMAGE
//
// MessageText:
//
//  v27 v4 322%0
//
#define AICF_SAR_HEAVY_DAMAGE            0x00000791L

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 323 <we are under fire, we need some cover!>
// v27 v4 323
//
// MessageId: AICF_SAR_NEED_COVER
//
// MessageText:
//
//  v27 v4 323%0
//
#define AICF_SAR_NEED_COVER              0x00000792L

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 324 <We're hit, need some help here!>
// v27 v4 324
//
// MessageId: AICF_SAR_NEED_HELP
//
// MessageText:
//
//  v27 v4 324%0
//
#define AICF_SAR_NEED_HELP               0x00000793L

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 325 Mayday, mayday, we are going down!
// v27 v4 325
//
// MessageId: AICF_SAR_MAYDAY_DOWN
//
// MessageText:
//
//  v27 v4 325%0
//
#define AICF_SAR_MAYDAY_DOWN             0x00000794L

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 326 <We are going down.  I say again, we are going down.>
// v27 v4 326
//
// MessageId: AICF_SAR_GOING_DOWN
//
// MessageText:
//
//  v27 v4 326%0
//
#define AICF_SAR_GOING_DOWN              0x00000795L

//
// v27 AWACS callsign
//
// v4 SAR callsign
//
// 327 <Mayday!  Mayday!  We've lost control!>
// v27 v4 327
//
// MessageId: AICF_SAR_MAYDAY
//
// MessageText:
//
//  v27 v4 327%0
//
#define AICF_SAR_MAYDAY                  0x00000796L

//
// v27 Player callsign
//
// v2 fighter callsign
//
// 585 <we're beginning our sweep of the target area now.>
// v27 v2 585
//
// MessageId: AICF_CAP_BEGINNING_SWEEP
//
// MessageText:
//
//  v27 v2 585%0
//
#define AICF_CAP_BEGINNING_SWEEP         0x00000797L

//
// v1 fighter callsign
//
// 618 <Tally bandit>
// v1 618
//
// MessageId: AICF_CAP_TALLY_BANDIT
//
// MessageText:
//
//  v1 618%0
//
#define AICF_CAP_TALLY_BANDIT            0x00000798L

//
// v1 fighter callsign
//
// 619 <Tally bandits>
// v1 619
//
// MessageId: AICF_CAP_TALLY_BANDITS
//
// MessageText:
//
//  v1 619%0
//
#define AICF_CAP_TALLY_BANDITS           0x00000799L

//
// v1 fighter callsign
//
// 621 <Engaging bandit>
// v1 621
//
// MessageId: AICF_CAP_ENGAGING_BANDIT
//
// MessageText:
//
//  v1 621%0
//
#define AICF_CAP_ENGAGING_BANDIT         0x0000079AL

//
// v1 fighter callsign
//
// 622 <Engaging bandits>
// v1 622
//
// MessageId: AICF_CAP_ENGAGING_BANDITS
//
// MessageText:
//
//  v1 622%0
//
#define AICF_CAP_ENGAGING_BANDITS        0x0000079BL

//
// v1 fighter callsign
//
// 624 <Intercepting incomming bandit>
// v1 624
//
// MessageId: AICF_CAP_INTERCEPT_BANDIT
//
// MessageText:
//
//  v1 624%0
//
#define AICF_CAP_INTERCEPT_BANDIT        0x0000079CL

//
// v1 fighter callsign
//
// 623 <Intercepting incomming bandits>
// v1 623
//
// MessageId: AICF_CAP_INTERCEPT_BANDITS
//
// MessageText:
//
//  v1 623%0
//
#define AICF_CAP_INTERCEPT_BANDITS       0x0000079DL

//
// v4 fighter callsign intense
//
// 200 <Splash One.>
// Not Translated
//
// MessageId: AICF_CAP_SPLASH_ONE
//
// MessageText:
//
//  v4 200%0
//
#define AICF_CAP_SPLASH_ONE              0x0000079EL

//
// v4 fighter callsign intense
//
// 201 <Bandit killed!>
// Not Translated
//
// MessageId: AICF_CAP_BANDIT_KILLED
//
// MessageText:
//
//  v4 201%0
//
#define AICF_CAP_BANDIT_KILLED           0x0000079FL

//
// v5 plane callsign and number intense.
//
// 516 <engaged defensive SAM>
// v5 516
//
// MessageId: AICF_CAP_ENGAGED_SAM
//
// MessageText:
//
//  v5 516%0
//
#define AICF_CAP_ENGAGED_SAM             0x000007A0L

//
// v30 AWACS callsign intense
//
// v4 fighter callsign intense
//
// 508 <is hit ejecting>
// Not Translated
//
// MessageId: AICF_CAP_EJECTING
//
// MessageText:
//
//  v30 v4 508%0
//
#define AICF_CAP_EJECTING                0x000007A1L

//
// v27 AWACS callsign
//
// v1 fighter callsign
//
// 509 <bingo fuel, heading for home.>
// Not Translated
//
// MessageId: AICF_CAP_FUEL_HOME
//
// MessageText:
//
//  v27 v1 509%0
//
#define AICF_CAP_FUEL_HOME               0x000007A2L

//
// v27 AWACS callsign
//
// v1 fighter callsign
//
// 510 <bingo fuel, heading for tanker.>
// Not Translated
//
// MessageId: AICF_CAP_FUEL_TANKER
//
// MessageText:
//
//  v27 v1 510%0
//
#define AICF_CAP_FUEL_TANKER             0x000007A3L

//
// v1 fighter callsign
//
// 711 < Winchester>
// v1 711
//
// MessageId: AICF_CAP_WINCHESTER
//
// MessageText:
//
//  v1 711%0
//
#define AICF_CAP_WINCHESTER              0x000007A4L

//
// v27 Player callsign
//
// v1 fighter callsign
//
// 512 <you're clear, returning to base.>
// Not Translated
//
// MessageId: AICF_CAP_RTB
//
// MessageText:
//
//  v27 v1 512%0
//
#define AICF_CAP_RTB                     0x000007A5L

//
// v27 Player callsign
//
// v1 fighter callsign
//
// 757 <we're headed for home.>
// v27 v1 757
//
// MessageId: AICF_CAP_HEADED_HOME
//
// MessageText:
//
//  v27 v1 757%0
//
#define AICF_CAP_HEADED_HOME             0x000007A6L

//
// v27 AWACS callsign
//
// v1 fighter callsign
//
// 583 <contact>
//
// v38 speed (fast/slow mover)
//
// v20 size (group/single)
//
// v9 cardinal direction <bullseye> 
//
// v10 range (# miles)
//
// v11 altitude (high/med/low)
// v27 v1 583 v38 v20 v9 v10 v11
//
// MessageId: AICF_CAP_CONTACT
//
// MessageText:
//
//  v27 v1 583 v38 v20 v9 v10 v11%0
//
#define AICF_CAP_CONTACT                 0x000007A7L

//
// v27 AWACS callsign
//
// v1 fighter callsign
//
// v38 speed (fast/slow mover)
//
// v20 size (group/single)
//
// 515 <radar contact bearing>
//
// v7 bearing in degrees
//
// v10 range (# miles)
//
// v11 altitude (high/med/low)
// Not Translated
//
// MessageId: AICF_CAP_CONTACT_BEARING
//
// MessageText:
//
//  v27 v1  v38 v20 515 v7 v10 v11%0
//
#define AICF_CAP_CONTACT_BEARING         0x000007A8L

//
// v27 AWACS callsign
//
// v1 fighter callsign
//
// 586 <clean>
// v27 v1 586
//
// MessageId: AICF_CAP_CLEAN
//
// MessageText:
//
//  v27 v1 586%0
//
#define AICF_CAP_CLEAN                   0x000007A9L

//
// v27 Player callsign
//
// v1 SEAD callsign
//
// 757 <we're headed for home.>
// v27 v1 757
//
// MessageId: AICF_SEAD_HEADED_HOME
//
// MessageText:
//
//  v27 v1 757%0
//
#define AICF_SEAD_HEADED_HOME            0x000007AAL

//
// 318 <Impact!>
// Not Translated
//
// MessageId: AICF_WSO_IMPACT
//
// MessageText:
//
//  318%0
//
#define AICF_WSO_IMPACT                  0x000007ABL

//
// 319 <Good hit!>
// Not Translated
//
// MessageId: AICF_WSO_GOOD_HIT
//
// MessageText:
//
//  319%0
//
#define AICF_WSO_GOOD_HIT                0x000007ACL

//
// 320 <Shack!>
// Not Translated
//
// MessageId: AICF_WSO_SHACK
//
// MessageText:
//
//  320%0
//
#define AICF_WSO_SHACK                   0x000007ADL

//
// 321 <Boom!  Oh yes!>
// Not Translated
//
// MessageId: AICF_WSO_BOOM
//
// MessageText:
//
//  321%0
//
#define AICF_WSO_BOOM                    0x000007AEL

//
// 322 <Missed, damn it!>
// Not Translated
//
// MessageId: AICF_WSO_MISSED_DAMN
//
// MessageText:
//
//  322%0
//
#define AICF_WSO_MISSED_DAMN             0x000007AFL

//
// 323 <Looks like the bombs missed>
// Not Translated
//
// MessageId: AICF_WSO_LOOKS_MISSED
//
// MessageText:
//
//  323%0
//
#define AICF_WSO_LOOKS_MISSED            0x000007B0L

//
// 324 <That sucks, we missed.>
// Not Translated
//
// MessageId: AICF_WSO_MISSED_SUCKS
//
// MessageText:
//
//  324%0
//
#define AICF_WSO_MISSED_SUCKS            0x000007B1L

//
// 325 <Look at those secondaries>
// Not Translated
//
// MessageId: AICF_WSO_LOOK_SECONDARIES
//
// MessageText:
//
//  325%0
//
#define AICF_WSO_LOOK_SECONDARIES        0x000007B2L

//
// 326 <We've got secondaires>
// Not Translated
//
// MessageId: AICF_WSO_GOT_SECONDARIES
//
// MessageText:
//
//  326%0
//
#define AICF_WSO_GOT_SECONDARIES         0x000007B3L

//
// 327 <Burn baby, burn>
// Not Translated
//
// MessageId: AICF_WSO_BURN
//
// MessageText:
//
//  327%0
//
#define AICF_WSO_BURN                    0x000007B4L

//
// 328 <Boom Boom!  Out go the lights!>
// Not Translated
//
// MessageId: AICF_WSO_BOOM_BOOM
//
// MessageText:
//
//  328%0
//
#define AICF_WSO_BOOM_BOOM               0x000007B5L

//
// 329 <We destroyed that bigger than Dallas!
// Not Translated
//
// MessageId: AICF_WSO_DESTROYED_DALLAS
//
// MessageText:
//
//  329%0
//
#define AICF_WSO_DESTROYED_DALLAS        0x000007B6L

//
// 330 <Wow!  Look at that!>
// Not Translated
//
// MessageId: AICF_WSO_WOW_LOOK
//
// MessageText:
//
//  330%0
//
#define AICF_WSO_WOW_LOOK                0x000007B7L

//
// v4 <This is> AWACS callsign intense
//
// 320 <Missiles inbound!>
// v4 320
//
// MessageId: AICF_AWACS_MISSILES_INBOUND
//
// MessageText:
//
//  v4 320%0
//
#define AICF_AWACS_MISSILES_INBOUND      0x000007B8L

//
// v4 <This is> AWACS callsign intense
//
// 321 <Shutting down and running from bandits!>
// v4 321
//
// MessageId: AICF_AWACS_RUNNING
//
// MessageText:
//
//  v4 321%0
//
#define AICF_AWACS_RUNNING               0x000007B9L

//
// v4 <This is> AWACS callsign intense
//
// 322 <we've been hit, going in!>
// v4 322
//
// MessageId: AICF_AWACS_GOING_IN
//
// MessageText:
//
//  v4 322%0
//
#define AICF_AWACS_GOING_IN              0x000007BAL

//
// v4 <This is> AWACS callsign intense
//
// 323 <MAYDAY MAYDAY!>
// v4 323
//
// MessageId: AICF_AWACS_MAYDAY
//
// MessageText:
//
//  v4 323%0
//
#define AICF_AWACS_MAYDAY                0x000007BBL

//
// v4 <This is> AWACS callsign intense
//
// 324 <we're hit, send help!>
// v4 324
//
// MessageId: AICF_AWACS_HIT_SEND_HELP
//
// MessageText:
//
//  v4 324%0
//
#define AICF_AWACS_HIT_SEND_HELP         0x000007BCL

//
// v27 Player callsign
//
// v1 bomber callsign
//
// 500 <tally on you, forming up>
// Not Translated
//
// MessageId: AICF_BOMBER_TALLY_FORM
//
// MessageText:
//
//  v27 v1 500%0
//
#define AICF_BOMBER_TALLY_FORM           0x000007BDL

//
// v27 Player callsign
//
// v1 bomber callsign
//
// 501 <forming up.>
// Not Translated
//
// MessageId: AICF_BOMBER_FORM_UP
//
// MessageText:
//
//  v27 v1 501%0
//
#define AICF_BOMBER_FORM_UP              0x000007BEL

//
// v27 Player callsign
//
// v1 bomber callsign
//
// 502 <breaking to attack.>
// Not Translated
//
// MessageId: AICF_BOMBER_BREAK_TO_ATTACK
//
// MessageText:
//
//  v27 v1 502%0
//
#define AICF_BOMBER_BREAK_TO_ATTACK      0x000007BFL

//
// v27 Player callsign
//
// v1 bomber callsign
//
// 503 <commencing attack run.>
// Not Translated
//
// MessageId: AICF_BOMBER_COMMENCE_ATTACK
//
// MessageText:
//
//  v27 v1 503%0
//
#define AICF_BOMBER_COMMENCE_ATTACK      0x000007C0L

//
// v27 Player callsign
//
// v1 bomber callsign
//
// 504 <reforming.>
// Not Translated
//
// MessageId: AICF_BOMBER_REFORMING
//
// MessageText:
//
//  v27 v1 504%0
//
#define AICF_BOMBER_REFORMING            0x000007C1L

//
// v27 Player callsign
//
// v1 bomber callsign
//
// 505 <reforming on you.>
// Not Translated
//
// MessageId: AICF_BOMBER_REFORMING_YOU
//
// MessageText:
//
//  v27 v1 505%0
//
#define AICF_BOMBER_REFORMING_YOU        0x000007C2L

//
// v27 Player callsign
//
// v1 bomber callsign
//
// 506 <visual on you, glad you're with us.>
// Not Translated
//
// MessageId: AICF_BOMBER_VISUAL_GLAD
//
// MessageText:
//
//  v27 v1 506%0
//
#define AICF_BOMBER_VISUAL_GLAD          0x000007C3L

//
// 200 <Bandits>
//
// v37 Clock position
// Not Translated
//
// MessageId: AICF_BOMBER_BANDITS
//
// MessageText:
//
//  200 v37%0
//
#define AICF_BOMBER_BANDITS              0x000007C4L

//
// 201 <Heads up, Bandits>
//
// v37 Clock position
// Not Translated
//
// MessageId: AICF_BOMBER_HEADS_UP_BANDITS
//
// MessageText:
//
//  201 v37%0
//
#define AICF_BOMBER_HEADS_UP_BANDITS     0x000007C5L

//
// v30 Player callsign intense
//
// v4 bomber callsign intense
//
// 202 <We're hit!>
// Not Translated
//
// MessageId: AICF_BOMBER_WE_ARE_HIT
//
// MessageText:
//
//  v30 v4 202%0
//
#define AICF_BOMBER_WE_ARE_HIT           0x000007C6L

//
// v30 Player callsign intense
//
// v4 bomber callsign intense
//
// 203 <taking damage!>
// Not Translated
//
// MessageId: AICF_BOMBER_DAMAGE
//
// MessageText:
//
//  v30 v4 203%0
//
#define AICF_BOMBER_DAMAGE               0x000007C7L

//
// v4 bomber callsign intense
//
// 204 <is going in!  Mayday!>
// Not Translated
//
// MessageId: AICF_BOMBER_MAYDAY
//
// MessageText:
//
//  v4 204%0
//
#define AICF_BOMBER_MAYDAY               0x000007C8L

//
// v4 bomber callsign intense
//
// 205 <ejecting now!>
// Not Translated
//
// MessageId: AICF_BOMBER_EJECT
//
// MessageText:
//
//  v4 205%0
//
#define AICF_BOMBER_EJECT                0x000007C9L

//
// v1 bomber callsign
//
// 698 <Bombs gone.>
// v1 698
//
// MessageId: AICF_BOMBER_BOMBS_GONE
//
// MessageText:
//
//  v1 698%0
//
#define AICF_BOMBER_BOMBS_GONE           0x000007CAL

//
// v1 bomber callsign
//
// 699 <Bombs away.>
// v1 699
//
// MessageId: AICF_BOMBER_BOMBS_AWAY
//
// MessageText:
//
//  v1 699%0
//
#define AICF_BOMBER_BOMBS_AWAY           0x000007CBL

//
// v27 Player callsign
//
// v1 bomber callsign
//
// 509 <Thanks for the escort, heading home.>
// Not Translated
//
// MessageId: AICF_BOMBER_THANKS
//
// MessageText:
//
//  v27 v1 509%0
//
#define AICF_BOMBER_THANKS               0x000007CCL

//
// v27 Player callsign
//
// v1 bomber callsign
//
// 510 <Returning to base.>
// Not Translated
//
// MessageId: AICF_BOMBER_RTB
//
// MessageText:
//
//  v27 v1 510%0
//
#define AICF_BOMBER_RTB                  0x000007CDL

//
// v27 AWACS callsign
//
// v1 SEAD callsign
//
// 562 <Mud spike>
//
// v7 bearing 
// v27 v1 562 v7
//
// MessageId: AICF_SEAD_MUD_SPIKE
//
// MessageText:
//
//  v27 v1 562 v7%0
//
#define AICF_SEAD_MUD_SPIKE              0x000007CEL

//
// v27 AWACS callsign
//
// v1 SEAD callsign
//
// 572 <Buddy spike>
//
// v7 bearing 
// v27 v1 572 v7
//
// MessageId: AICF_SEAD_BUDDY_SPIKE
//
// MessageText:
//
//  v27 v1 572 v7%0
//
#define AICF_SEAD_BUDDY_SPIKE            0x000007CFL

//
// v27 AWACS callsign
//
// v1 SEAD callsign
//
// 503 <Spike>
//
// v7 bearing 
// Not Translated
//
// MessageId: AICF_SEAD_SPIKE
//
// MessageText:
//
//  v27 v1 503 v7%0
//
#define AICF_SEAD_SPIKE                  0x000007D0L

//
// v27 AWACS callsign
//
// v1 SEAD callsign
//
// 573 <Naked>
// v27 v1 573
//
// MessageId: AICF_SEAD_NAKED
//
// MessageText:
//
//  v27 v1 573%0
//
#define AICF_SEAD_NAKED                  0x000007D1L

//
// 577 <Magnum!>
// 577
//
// MessageId: AICF_SEAD_MAGNUM
//
// MessageText:
//
//  577%0
//
#define AICF_SEAD_MAGNUM                 0x000007D2L

//
// v27 AWACS callsign
//
// v1 SEAD callsign
//
// 506 <attacking SAM launcher>
// Not Translated
//
// MessageId: AICF_SEAD_ATTACKING_SAM
//
// MessageText:
//
//  v27 v1 506%0
//
#define AICF_SEAD_ATTACKING_SAM          0x000007D3L

//
// v27 AWACS callsign
//
// v1 SEAD callsign
//
// 507 <attacking Triple-A site.>
// Not Translated
//
// MessageId: AICF_SEAD_ATTACKING_AAA
//
// MessageText:
//
//  v27 v1 507%0
//
#define AICF_SEAD_ATTACKING_AAA          0x000007D4L

//
// v27 AWACS callsign
//
// v1 SEAD callsign
//
// 508 <Winchester, returning to base.>
// Not Translated
//
// MessageId: AICF_SEAD_WINCHESTER
//
// MessageText:
//
//  v27 v1 508%0
//
#define AICF_SEAD_WINCHESTER             0x000007D5L

//
// v5 SEAD callsign intense
//
// 526 <Music on.>
// v5 526
//
// MessageId: AICF_SEAD_MUSIC_ON
//
// MessageText:
//
//  v5 526%0
//
#define AICF_SEAD_MUSIC_ON               0x000007D6L

//
// v27 AWACS callsign
//
// v2 SEAD callsign
//
// 527 <music off.>
// v2 527
//
// MessageId: AICF_SEAD_MUSIC_OFF
//
// MessageText:
//
//  v2 527%0
//
#define AICF_SEAD_MUSIC_OFF              0x000007D7L

//
// v4 SEAD callsign intense
//
// 515 <is ejecting now!>
// Not Translated
//
// MessageId: AICF_SEAD_EJECTING
//
// MessageText:
//
//  v4 515%0
//
#define AICF_SEAD_EJECTING               0x000007D8L

//
// v25 Flight callsign
//
// 552 <check>
// Not Translated
//
// MessageId: AICF_WSO_CHECK_CALL
//
// MessageText:
//
//  v25 552%0
//
#define AICF_WSO_CHECK_CALL              0x000007D9L

//
// v25 Flight callsign
//
// 553 <stores check.>
// Not Translated
//
// MessageId: AICF_WSO_STORES_CHECK
//
// MessageText:
//
//  v25 553%0
//
#define AICF_WSO_STORES_CHECK            0x000007DAL

//
// v25 Flight callsign
//
// 554 <showtime, green 'em up.>
// Not Translated
//
// MessageId: AICF_WSO_SHOWTIME
//
// MessageText:
//
//  v25 554%0
//
#define AICF_WSO_SHOWTIME                0x000007DBL

//
// v25 Flight callsign
//
// 612 <cleared hot.>
// v25 612
//
// MessageId: AICF_WSO_CLEARED_HOT
//
// MessageText:
//
//  v25 612%0
//
#define AICF_WSO_CLEARED_HOT             0x000007DCL

//
// v25 Flight callsign
//
// 613 <commence attack.>
// v25 613
//
// MessageId: AICF_WSO_COMMENCE_ATTACK
//
// MessageText:
//
//  v25 613%0
//
#define AICF_WSO_COMMENCE_ATTACK         0x000007DDL

//
// v27 bomber callsign
//
// v1 Player callsign
//
// 560 <we have you in sight, forming up.>
// Not Translated
//
// MessageId: AICF_WSO_FORM_BOMBER
//
// MessageText:
//
//  v27 v1 560%0
//
#define AICF_WSO_FORM_BOMBER             0x000007DEL

//
// v27 bomber callsign
//
// v1 Player callsign
//
// 561 <on our way.>
// Not Translated
//
// MessageId: AICF_WSO_ON_WAY
//
// MessageText:
//
//  v27 v1 561%0
//
#define AICF_WSO_ON_WAY                  0x000007DFL

//
// v35 Ground call sign
//
// v1 Player callsign
//
// 561 <on our way.>
// Not Translated
//
// MessageId: AICF_WSO_GRND_ON_WAY
//
// MessageText:
//
//  v35 v1 561%0
//
#define AICF_WSO_GRND_ON_WAY             0x000007E0L

//
// v27 bomber callsign
//
// v1 Player callsign
//
// 562 <we're with you.>
// Not Translated
//
// MessageId: AICF_WSO_WITH_YOU
//
// MessageText:
//
//  v27 v1 562%0
//
#define AICF_WSO_WITH_YOU                0x000007E1L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 300 <copy, standby.>
// v27 v1 300
//
// MessageId: AICF_JSTARS_COPY_STANDBY
//
// MessageText:
//
//  v27 v1 300%0
//
#define AICF_JSTARS_COPY_STANDBY         0x000007E2L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 301 <copy, cleared to patrol as fragged.>
// v27 v1 301
//
// MessageId: AICF_JSTARS_COPY_CLEARED_PATROL
//
// MessageText:
//
//  v27 v1 301%0
//
#define AICF_JSTARS_COPY_CLEARED_PATROL  0x000007E3L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 302 <copy, good hunting.>
// v27 v1 302
//
// MessageId: AICF_JSTARS_COPY_GOOD_HUNT
//
// MessageText:
//
//  v27 v1 302%0
//
#define AICF_JSTARS_COPY_GOOD_HUNT       0x000007E4L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 303 <copy, possible target located, standby for words.>
// v27 v1 303
//
// MessageId: AICF_JSTARS_POSSIBLE_TARG
//
// MessageText:
//
//  v27 v1 303%0
//
#define AICF_JSTARS_POSSIBLE_TARG        0x000007E5L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 304 <ground targets located at>
//
// v8 cardinal w bullseye
//
// v10 range
// v27 v1 304 v43 v10
//
// MessageId: AICF_JSTARS_GROUND_AT
//
// MessageText:
//
//  v27 v1 304 v8 v10%0
//
#define AICF_JSTARS_GROUND_AT            0x000007E6L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 305 <movers at>
//
// v8 cardinal w bullseye
//
// v10 range
// v27 v1 305 v43 v10
//
// MessageId: AICF_JSTARS_MOVERS_AT
//
// MessageText:
//
//  v27 v1 305 v8 v10%0
//
#define AICF_JSTARS_MOVERS_AT            0x000007E7L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 306 <vehicle traffic at>
//
// v8 cardinal w bullseye
//
// v10 range
// v27 v1 306 v43 v10
//
// MessageId: AICF_JSTARS_VEHICLES_AT
//
// MessageText:
//
//  v27 v1 306 v8 v10%0
//
#define AICF_JSTARS_VEHICLES_AT          0x000007E8L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 307 <ground targets bearing>
//
// v7 bearing
//
// v10 range
// v27 v1 307 v7 v10
//
// MessageId: AICF_JSTARS_GROUND_BEARING
//
// MessageText:
//
//  v27 v1 307 v7 v10%0
//
#define AICF_JSTARS_GROUND_BEARING       0x000007E9L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 308 <movers bearing>
//
// v7 bearing
//
// v10 range
// v27 v1 308 v7 v10
//
// MessageId: AICF_JSTARS_MOVERS_BEARING
//
// MessageText:
//
//  v27 v1 308 v7 v10%0
//
#define AICF_JSTARS_MOVERS_BEARING       0x000007EAL

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 309 <vehicle traffic bearing>
//
// v7 bearing
//
// v10 range
// v27 v1 309 v7 v10
//
// MessageId: AICF_JSTARS_VEHCILES_BEARING
//
// MessageText:
//
//  v27 v1 309 v7 v10%0
//
#define AICF_JSTARS_VEHCILES_BEARING     0x000007EBL

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 310 <area appears clear.>
// v27 v1 310
//
// MessageId: AICF_JSTARS_AREA_CLEAR
//
// MessageText:
//
//  v27 v1 310%0
//
#define AICF_JSTARS_AREA_CLEAR           0x000007ECL

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 311 <ambiguous returns, standby.>
// v27 v1 311
//
// MessageId: AICF_JSTARS_UNSURE
//
// MessageText:
//
//  v27 v1 311%0
//
#define AICF_JSTARS_UNSURE               0x000007EDL

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 312 <gadget bent, you are on your own.>
// v27 v1 312
//
// MessageId: AICF_JSTARS_BROKEN
//
// MessageText:
//
//  v27 v1 312%0
//
#define AICF_JSTARS_BROKEN               0x000007EEL

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 313 <acknowledged>
// v27 v1 313
//
// MessageId: AICF_JSTARS_ACKNOWLEDGED
//
// MessageText:
//
//  v27 v1 313%0
//
#define AICF_JSTARS_ACKNOWLEDGED         0x000007EFL

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 314 <copy>
// v27 v1 314
//
// MessageId: AICF_JSTARS_COPY
//
// MessageText:
//
//  v27 v1 314%0
//
#define AICF_JSTARS_COPY                 0x000007F0L

//
// v27 Player callsign
//
// v1 JSTARS callsign
//
// 315 <negative>
// v27 v1 315
//
// MessageId: AICF_JSTARS_NEGATIVE
//
// MessageText:
//
//  v27 v1 315%0
//
#define AICF_JSTARS_NEGATIVE             0x000007F1L

//
// v1 <This is> JSTARS callsign
//
// 316 <we are under attack!>
// v1 316
//
// MessageId: AICF_JSTARS_UNDER_ATTACK
//
// MessageText:
//
//  v1 316%0
//
#define AICF_JSTARS_UNDER_ATTACK         0x000007F2L

//
// v1 <This is> JSTARS callsign
//
// 317 <bandits closing on us!>
// v1 317
//
// MessageId: AICF_JSTARS_BANDITS_CLOSING
//
// MessageText:
//
//  v1 317%0
//
#define AICF_JSTARS_BANDITS_CLOSING      0x000007F3L

//
// v1 <This is> JSTARS callsign
//
// 318 <we've been hit, going down!>
// v1 318
//
// MessageId: AICF_JSTARS_GOING_DOWN
//
// MessageText:
//
//  v1 318%0
//
#define AICF_JSTARS_GOING_DOWN           0x000007F4L

//
// v4 <This is> JSTARS callsign intense
//
// 203 <MAYDAY MAYDAY!>
// Not Translated
//
// MessageId: AICF_JSTARS_MAYDAY
//
// MessageText:
//
//  v4 203%0
//
#define AICF_JSTARS_MAYDAY               0x000007F5L

//
// v1 <This is> JSTARS callsign
//
// 319 <we're hit, send help!>
// v1 319
//
// MessageId: AICF_JSTARS_SEND_HELP
//
// MessageText:
//
//  v1 319%0
//
#define AICF_JSTARS_SEND_HELP            0x000007F6L

//
// v27 JSTARS callsign
//
// v25 flight callsign
//
// 665 <checking in>
// v27 v25 665
//
// MessageId: AICF_WSO_J_CHECK_IN
//
// MessageText:
//
//  v27 v25 665%0
//
#define AICF_WSO_J_CHECK_IN              0x000007F7L

//
// v27 JSTARS callsign
//
// v25 flight callsign
//
// 666 <available for tasking>
// v27 v25 666
//
// MessageId: AICF_WSO_J_AVAILABLE
//
// MessageText:
//
//  v27 v25 666%0
//
#define AICF_WSO_J_AVAILABLE             0x000007F8L

//
// v27 JSTARS callsign
//
// v25 flight callsign
//
// 667 <request new target>
// v27 v25 667
//
// MessageId: AICF_WSO_J_REQUEST_TARGET
//
// MessageText:
//
//  v27 v25 667%0
//
#define AICF_WSO_J_REQUEST_TARGET        0x000007F9L

//
// v27 JSTARS callsign
//
// v25 flight callsign
//
// 668 <we are heading home>
// v27 v25 668
//
// MessageId: AICF_WSO_J_HEADING_HOME
//
// MessageText:
//
//  v27 v25 668%0
//
#define AICF_WSO_J_HEADING_HOME          0x000007FAL

//
// v27 JSTARS callsign
//
// v25 flight callsign
//
// 669 <returning to base>
// v27 v25 669
//
// MessageId: AICF_WSO_J_RTB
//
// MessageText:
//
//  v27 v25 669%0
//
#define AICF_WSO_J_RTB                   0x000007FBL

//
// v3 wingman number
//
// 528 <targets in sight, in hot>
// v3 528
//
// MessageId: AICF_WING_TARGETS_SIGHTED_IN_HOT
//
// MessageText:
//
//  v3 528%0
//
#define AICF_WING_TARGETS_SIGHTED_IN_HOT 0x000007FCL

//
// v3 wingman number
//
// 210 <gomers spotted, engaging>
// v3 529
//
// MessageId: AICF_WING_EGAGING_GOMERS
//
// MessageText:
//
//  v3 529%0
//
#define AICF_WING_EGAGING_GOMERS         0x000007FDL

//
// v2 wingman callsign
//
// 540 <ground targets destroyed>
// v2 540
//
// MessageId: AICF_WING_GROUND_DESTROYED
//
// MessageText:
//
//  v2 540%0
//
#define AICF_WING_GROUND_DESTROYED       0x000007FEL

//
// v2 wingman callsign
//
// 541 <target destroyed, reforming>
// v2 541
//
// MessageId: AICF_WING_TARGET_DESTROYED_REFORMING
//
// MessageText:
//
//  v2 541%0
//
#define AICF_WING_TARGET_DESTROYED_REFORMING 0x000007FFL

//
// v2 wingman callsign
//
// 614 <request permission to engage.>
// v2 614
//
// MessageId: AICF_ENGAGE_READY_MSG_1
//
// MessageText:
//
//  v2 614%0
//
#define AICF_ENGAGE_READY_MSG_1          0x00000800L

//
// v2 wingman callsign
//
// 615 <ready to engage.>
// v2 615
//
// MessageId: AICF_ENGAGE_READY_MSG_2
//
// MessageText:
//
//  v2 615%0
//
#define AICF_ENGAGE_READY_MSG_2          0x00000801L

//
// v2 wingman callsign
//
// 616 <request permission to attack>
// v2 616
//
// MessageId: AICF_WING_REQUEST_ATTACK
//
// MessageText:
//
//  v2 616%0
//
#define AICF_WING_REQUEST_ATTACK         0x00000802L

//
// v2 wingman callsign
//
// 617 <ready to start out attack run>
// v2 617
//
// MessageId: AICF_WING_READY_TO_ATTACK
//
// MessageText:
//
//  v2 617%0
//
#define AICF_WING_READY_TO_ATTACK        0x00000803L

//
// v3 wingman number
//
// 758 <spreading out>
// v3 758
//
// MessageId: AICF_SPREADING_OUT
//
// MessageText:
//
//  v3 758%0
//
#define AICF_SPREADING_OUT               0x00000804L

//
// v3 wingman number
//
// 759 <closing up.>
// v3 759
//
// MessageId: AICF_CLOSING_UP
//
// MessageText:
//
//  v3 759%0
//
#define AICF_CLOSING_UP                  0x00000805L

//
// v3 wingman number
//
// 760 <copy, anchoring here.>
// v3 760
//
// MessageId: AICF_ORBIT_RESPONSE_1
//
// MessageText:
//
//  v3 760%0
//
#define AICF_ORBIT_RESPONSE_1            0x00000806L

//
// v3 wingman number
//
// 761 <roger, obiting here.>
// v3 761
//
// MessageId: AICF_ORBIT_RESPONSE_2
//
// MessageText:
//
//  v3 761%0
//
#define AICF_ORBIT_RESPONSE_2            0x00000807L

//
// v3 wingman number
//
// 762 <copy, moving to cover.>
// v3 762
//
// MessageId: AICF_COVER_RESPONSE_1
//
// MessageText:
//
//  v3 762%0
//
#define AICF_COVER_RESPONSE_1            0x00000808L

//
// v3 wingman number
//
// 763 <roger, covering>
// v3 763
//
// MessageId: AICF_COVER_RESPONSE_2
//
// MessageText:
//
//  v3 763%0
//
#define AICF_COVER_RESPONSE_2            0x00000809L

//
// 209 <SAM launch>
//
// v37 clock position intense
// Not Translated
//
// MessageId: AICF_WSO_SAM_SPOT_1
//
// MessageText:
//
//  209 v37%0
//
#define AICF_WSO_SAM_SPOT_1              0x0000080AL

//
// 210 <SAM! SAM!>
//
// v37 clock position intense
// Not Translated
//
// MessageId: AICF_WSO_SAM_SPOT_2
//
// MessageText:
//
//  210 v37%0
//
#define AICF_WSO_SAM_SPOT_2              0x0000080BL

//
// 211 <Missile launch>
//
// v37 clock position intense
// Not Translated
//
// MessageId: AICF_WSO_SAM_SPOT_3
//
// MessageText:
//
//  211 v37%0
//
#define AICF_WSO_SAM_SPOT_3              0x0000080CL

//
// v5 plane callsign and number intense.
//
// 516 <is engaged defensive SAM>
// v5 516
//
// MessageId: AICF_WING_DEFENSIVE_SAM
//
// MessageText:
//
//  v5 516%0
//
#define AICF_WING_DEFENSIVE_SAM          0x0000080DL

//
// v5 plane callsign and number intense.
//
// 213 <is engaged defensive Roland>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_ROLAND
//
// MessageText:
//
//  v5 213%0
//
#define AICF_WING_DEFENSIVE_ROLAND       0x0000080EL

//
// v5 plane callsign and number intense.
//
// 214 <is engaged defensive Hawk>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_HAWK
//
// MessageText:
//
//  v5 214%0
//
#define AICF_WING_DEFENSIVE_HAWK         0x0000080FL

//
// v5 plane callsign and number intense.
//
// 215 <is engaged defensive SA-2>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_SA_2
//
// MessageText:
//
//  v5 215%0
//
#define AICF_WING_DEFENSIVE_SA_2         0x00000810L

//
// v5 plane callsign and number intense.
//
// 216 <is engaged defensive SA-3>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_SA_3
//
// MessageText:
//
//  v5 216%0
//
#define AICF_WING_DEFENSIVE_SA_3         0x00000811L

//
// v5 plane callsign and number intense.
//
// 217 <is engaged defensive SA-6>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_SA_6
//
// MessageText:
//
//  v5 217%0
//
#define AICF_WING_DEFENSIVE_SA_6         0x00000812L

//
// v5 plane callsign and number intense.
//
// 218 <is engaged defensive SA-7>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_SA_7
//
// MessageText:
//
//  v5 218%0
//
#define AICF_WING_DEFENSIVE_SA_7         0x00000813L

//
// v5 plane callsign and number intense.
//
// 219 <is engaged defensive SA-8>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_SA_8
//
// MessageText:
//
//  v5 219%0
//
#define AICF_WING_DEFENSIVE_SA_8         0x00000814L

//
// v5 plane callsign and number intense.
//
// 220 <is engaged defensive SA-9>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_SA_9
//
// MessageText:
//
//  v5 220%0
//
#define AICF_WING_DEFENSIVE_SA_9         0x00000815L

//
// v5 plane callsign and number intense.
//
// 221 <is engaged defensive SA-10>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_SA_10
//
// MessageText:
//
//  v5 221%0
//
#define AICF_WING_DEFENSIVE_SA_10        0x00000816L

//
// v5 plane callsign and number intense.
//
// 222 <is engaged defensive SA-13>
// Not Translated
//
// MessageId: AICF_WING_DEFENSIVE_SA_13
//
// MessageText:
//
//  v5 222%0
//
#define AICF_WING_DEFENSIVE_SA_13        0x00000817L

//
// v5 plane callsign and number intense.
//
// 512 <SAM launch>
//
// v37 clock position intense
// v5 512 v37
//
// MessageId: AICF_WING_SAM_SPOT_2
//
// MessageText:
//
//  v5 512 v37%0
//
#define AICF_WING_SAM_SPOT_2             0x00000818L

//
// v5 plane callsign and number intense.
//
// 513 <SAM>
//
// v37 clock position intense
// v5 513 v37
//
// MessageId: AICF_WING_SAM_SPOT_3
//
// MessageText:
//
//  v5 513 v37%0
//
#define AICF_WING_SAM_SPOT_3             0x00000819L

//
// 762 <scream>
// Not Translated
//
// MessageId: AICF_WSO_SCREAM
//
// MessageText:
//
//  762%0
//
#define AICF_WSO_SCREAM                  0x0000081AL

//
// 763 <scream>
// Not Translated
//
// MessageId: AICF_WSO_SCREAM_2
//
// MessageText:
//
//  763%0
//
#define AICF_WSO_SCREAM_2                0x0000081BL

//
// 277 <Spike>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WSO_SPIKE
//
// MessageText:
//
//  277 v24%0
//
#define AICF_WSO_SPIKE                   0x0000081CL

//
// 278 <Mud Spike at>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WSO_MUD_SPIKE
//
// MessageText:
//
//  278 v24%0
//
#define AICF_WSO_MUD_SPIKE               0x0000081DL

//
// 279 <Spike GCI at>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WSO_GCI_SPIKE
//
// MessageText:
//
//  279 v24%0
//
#define AICF_WSO_GCI_SPIKE               0x0000081EL

//
// 280 <Spike SAM at>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WSO_SAM_SPIKE
//
// MessageText:
//
//  280 v24%0
//
#define AICF_WSO_SAM_SPIKE               0x0000081FL

//
// 281 <Spike Triple A at>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WSO_AAA_SPIKE
//
// MessageText:
//
//  281 v24%0
//
#define AICF_WSO_AAA_SPIKE               0x00000820L

//
// 282 <Spike MiG at>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WSO_MIG_SPIKE
//
// MessageText:
//
//  282 v24%0
//
#define AICF_WSO_MIG_SPIKE               0x00000821L

//
// 284 <Mud Lock on>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WSO_MUD_LOCK
//
// MessageText:
//
//  284 v24%0
//
#define AICF_WSO_MUD_LOCK                0x00000822L

//
// 285 <Buddy Spike at>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_BUDDY_SPIKE
//
// MessageText:
//
//  285 v24%0
//
#define AICF_BUDDY_SPIKE                 0x00000823L

//
// v2 Wingman number
//
// 562 <has a Mud Spike at>
//
// v24 Clock position
// v2 562 v24
//
// MessageId: AICF_WING_RADAR_DETECT_1
//
// MessageText:
//
//  v2 562 v24%0
//
#define AICF_WING_RADAR_DETECT_1         0x00000824L

//
// v2 Wingman number
//
// 563 <Spike GCI at>
//
// v24 Clock position
// v2 563 v24
//
// MessageId: AICF_WING_RADAR_DETECT_2
//
// MessageText:
//
//  v2 563 v24%0
//
#define AICF_WING_RADAR_DETECT_2         0x00000825L

//
// v2 Wingman number
//
// 564 Spike SAM
//
// v24 Clock position
// v2 564 v24
//
// MessageId: AICF_WING_RADAR_DETECT_3
//
// MessageText:
//
//  v2 564 v24%0
//
#define AICF_WING_RADAR_DETECT_3         0x00000826L

//
// v2 Wingman number
//
// 565 <Spike Triple A>
//
// v24 Clock position
// v2 565 v24
//
// MessageId: AICF_WING_RADAR_DETECT_4
//
// MessageText:
//
//  v2 565 v24%0
//
#define AICF_WING_RADAR_DETECT_4         0x00000827L

//
// v2 Wingman number
//
// 566 <Spike MiG>
//
// v24 Clock position
// v2 566 v24
//
// MessageId: AICF_WING_RADAR_DETECT_5
//
// MessageText:
//
//  v2 566 v24%0
//
#define AICF_WING_RADAR_DETECT_5         0x00000828L

//
// v2 Wingman number
//
// 569 <Mud Lock on>
//
// v24 Clock position
// v2 569 v24
//
// MessageId: AICF_WING_RADAR_DETECT_7
//
// MessageText:
//
//  v2 569 v24%0
//
#define AICF_WING_RADAR_DETECT_7         0x00000829L

//
// v2 Wingman number
//
// 571 <Spike Spike>
//
// v24 Clock position
// v2 571 v24
//
// MessageId: AICF_WING_RADAR_DETECT_8
//
// MessageText:
//
//  v2 571 v24%0
//
#define AICF_WING_RADAR_DETECT_8         0x0000082AL

//
// v2 Wingman number
//
// 572 <Buddy Spike>
//
// v24 Clock position
// v2 572 v24
//
// MessageId: AICF_WING_RADAR_DETECT_9
//
// MessageText:
//
//  v2 572 v24%0
//
#define AICF_WING_RADAR_DETECT_9         0x0000082BL

//
// v2 Pilot callsign
//
// 496 <Fence in.>
// v2 496
//
// MessageId: AICF_WSO_FENCE_IN
//
// MessageText:
//
//  v2 496%0
//
#define AICF_WSO_FENCE_IN                0x0000082CL

//
// v2 Pilot callsign
//
// 497 <Fence out.>
// Not Translated
//
// MessageId: AICF_WSO_FENCE_OUT
//
// MessageText:
//
//  v2 497%0
//
#define AICF_WSO_FENCE_OUT               0x0000082DL

//
// 537 <Radar BIT runs clean.>
// Not Translated
//
// MessageId: AICF_WSO_RADAR_BIT_CLEAN
//
// MessageText:
//
//  537%0
//
#define AICF_WSO_RADAR_BIT_CLEAN         0x0000082EL

//
// 538 <Winders checkout ok>
// Not Translated
//
// MessageId: AICF_WSO_WINDERS_OK
//
// MessageText:
//
//  538%0
//
#define AICF_WSO_WINDERS_OK              0x0000082FL

//
// 539 <Sparrows check good>
// Not Translated
//
// MessageId: AICF_WSO_SPARROWS_GOOD
//
// MessageText:
//
//  539%0
//
#define AICF_WSO_SPARROWS_GOOD           0x00000830L

//
// 540 <Slammers look good>
// Not Translated
//
// MessageId: AICF_WSO_SLAMMERS_GOOD
//
// MessageText:
//
//  540%0
//
#define AICF_WSO_SLAMMERS_GOOD           0x00000831L

//
// 541 <Air to Mud stores look OK.>
// Not Translated
//
// MessageId: AICF_WSO_A2G_OK
//
// MessageText:
//
//  541%0
//
#define AICF_WSO_A2G_OK                  0x00000832L

//
// 542 <IFF set>
// Not Translated
//
// MessageId: AICF_WSO_IFF_SET
//
// MessageText:
//
//  542%0
//
#define AICF_WSO_IFF_SET                 0x00000833L

//
// 543 <Engines look good>
// Not Translated
//
// MessageId: AICF_WSO_ENGINES_GOOD
//
// MessageText:
//
//  543%0
//
#define AICF_WSO_ENGINES_GOOD            0x00000834L

//
// 544 <TEWS powered up>
// Not Translated
//
// MessageId: AICF_WSO_TEWS_UP
//
// MessageText:
//
//  544%0
//
#define AICF_WSO_TEWS_UP                 0x00000835L

//
// 545 <Radios set>
// Not Translated
//
// MessageId: AICF_WSO_RADIO_SET
//
// MessageText:
//
//  545%0
//
#define AICF_WSO_RADIO_SET               0x00000836L

//
// 546 <Check external lights off>
// Not Translated
//
// MessageId: AICF_WSO_CHECK_LIGHTS_OFF
//
// MessageText:
//
//  546%0
//
#define AICF_WSO_CHECK_LIGHTS_OFF        0x00000837L

//
// 547 <Platform looks tight.>
// Not Translated
//
// MessageId: AICF_WSO_PLATFORM_TIGHT
//
// MessageText:
//
//  547%0
//
#define AICF_WSO_PLATFORM_TIGHT          0x00000838L

//
// 258 <We've got a fuel leak!>
// Not Translated
//
// MessageId: AICF_WSO_FUEL_LEAK
//
// MessageText:
//
//  258%0
//
#define AICF_WSO_FUEL_LEAK               0x00000839L

//
// 259 <We are on fire!>
// Not Translated
//
// MessageId: AICF_WSO_ON_FIRE
//
// MessageText:
//
//  259%0
//
#define AICF_WSO_ON_FIRE                 0x0000083AL

//
// 260 <We're losing hydraulic pressure!>
// Not Translated
//
// MessageId: AICF_WSO_HYDRAULIC_DAMAGE
//
// MessageText:
//
//  260%0
//
#define AICF_WSO_HYDRAULIC_DAMAGE        0x0000083BL

//
// 261 <Engine One is damaged!>
// Not Translated
//
// MessageId: AICF_WSO_ENGINE_ONE_DAMAGE
//
// MessageText:
//
//  261%0
//
#define AICF_WSO_ENGINE_ONE_DAMAGE       0x0000083CL

//
// 262 <Engine Two is damaged!>
// Not Translated
//
// MessageId: AICF_WSO_ENGINE_TWO_DAMAGE
//
// MessageText:
//
//  262%0
//
#define AICF_WSO_ENGINE_TWO_DAMAGE       0x0000083DL

//
// 263 <We're in deep doo doo now!>
// Not Translated
//
// MessageId: AICF_WSO_DEEP_DOO
//
// MessageText:
//
//  263%0
//
#define AICF_WSO_DEEP_DOO                0x0000083EL

//
// 264 <Check you caution panel, we've lost some systems!>
// Not Translated
//
// MessageId: AICF_WSO_CHECK_PANNEL
//
// MessageText:
//
//  264%0
//
#define AICF_WSO_CHECK_PANNEL            0x0000083FL

//
// 265 <We're hurt bad!>
// Not Translated
//
// MessageId: AICF_WSO_HURT_BAD
//
// MessageText:
//
//  265%0
//
#define AICF_WSO_HURT_BAD                0x00000840L

//
// 266 <We're going in!  Eject!  Eject!>
// Not Translated
//
// MessageId: AICF_WSO_EJECT_EJECT
//
// MessageText:
//
//  266%0
//
#define AICF_WSO_EJECT_EJECT             0x00000841L

//
// 267 <What was that?>
// Not Translated
//
// MessageId: AICF_WSO_WHAT_WAS_THAT
//
// MessageText:
//
//  267%0
//
#define AICF_WSO_WHAT_WAS_THAT           0x00000842L

//
// 268 <We've lost the Tews!>
// Not Translated
//
// MessageId: AICF_WSO_LOST_TEWS
//
// MessageText:
//
//  268%0
//
#define AICF_WSO_LOST_TEWS               0x00000843L

//
// 269 <The radar is out!>
// Not Translated
//
// MessageId: AICF_WSO_RADAR_OUT
//
// MessageText:
//
//  269%0
//
#define AICF_WSO_RADAR_OUT               0x00000844L

//
// 270 <Looks like the central computer is gone.>
// Not Translated
//
// MessageId: AICF_WSO_COMPUTER_GONE
//
// MessageText:
//
//  270%0
//
#define AICF_WSO_COMPUTER_GONE           0x00000845L

//
// 271 <The targeting pod is dead!>
// Not Translated
//
// MessageId: AICF_WSO_TARGETING_DEAD
//
// MessageText:
//
//  271%0
//
#define AICF_WSO_TARGETING_DEAD          0x00000846L

//
// 272 <The Nav pod must have been hit!>
// Not Translated
//
// MessageId: AICF_WSO_NAV_HIT
//
// MessageText:
//
//  272%0
//
#define AICF_WSO_NAV_HIT                 0x00000847L

//
// 273 <The autopilot is shot!>
// Not Translated
//
// MessageId: AICF_WSO_AUTOPILOT_SHOT
//
// MessageText:
//
//  273%0
//
#define AICF_WSO_AUTOPILOT_SHOT          0x00000848L

//
// 274 <Pacs is out!>
// Not Translated
//
// MessageId: AICF_WSO_PACS_OUT
//
// MessageText:
//
//  274%0
//
#define AICF_WSO_PACS_OUT                0x00000849L

//
// 275 <We're losing cabin pressure!>
// Not Translated
//
// MessageId: AICF_WSO_CABIN_PRESSURE
//
// MessageText:
//
//  275%0
//
#define AICF_WSO_CABIN_PRESSURE          0x0000084AL

//
// 276 <Take us lower!  We've lost pressurization!>
// Not Translated
//
// MessageId: AICF_WSO_LOST_PRESSUREIZATION
//
// MessageText:
//
//  276%0
//
#define AICF_WSO_LOST_PRESSUREIZATION    0x0000084BL

//
// 202 <Triple A at>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WSO_AAA_SPOT_1
//
// MessageText:
//
//  202 v24%0
//
#define AICF_WSO_AAA_SPOT_1              0x0000084CL

//
// 203 <Guns below us at>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WSO_AAA_SPOT_2
//
// MessageText:
//
//  203 v24%0
//
#define AICF_WSO_AAA_SPOT_2              0x0000084DL

//
// 204 <Lots of Triple A left>
// Not Translated
//
// MessageId: AICF_WSO_AAA_LEFT
//
// MessageText:
//
//  204%0
//
#define AICF_WSO_AAA_LEFT                0x0000084EL

//
// 205 <Lots of Triple A right.>
// Not Translated
//
// MessageId: AICF_WSO_AAA_RIGHT
//
// MessageText:
//
//  205%0
//
#define AICF_WSO_AAA_RIGHT               0x0000084FL

//
// 206 <Lots of Triple A behind us.>
// Not Translated
//
// MessageId: AICF_WSO_AAA_BEHIND
//
// MessageText:
//
//  206%0
//
#define AICF_WSO_AAA_BEHIND              0x00000850L

//
// 207 <Lots of Triple A all around us.>
// Not Translated
//
// MessageId: AICF_WSO_AAA_AROUND
//
// MessageText:
//
//  207%0
//
#define AICF_WSO_AAA_AROUND              0x00000851L

//
// 208 <Lots of Triple A ahead.>
// Not Translated
//
// MessageId: AICF_WSO_AAA_AHEAD
//
// MessageText:
//
//  208%0
//
#define AICF_WSO_AAA_AHEAD               0x00000852L

//
// v2 plane callsign with number.
//
// 508 <triple A at>
//
// v24 Clock position
// v2 508 v24
//
// MessageId: AICF_WING_AAA_SPOT_1
//
// MessageText:
//
//  v2 508 v24%0
//
#define AICF_WING_AAA_SPOT_1             0x00000853L

//
// v2 plane callsign with number.
//
// 205 <ground fire at>
//
// v24 Clock position
// Not Translated
//
// MessageId: AICF_WING_AAA_SPOT_2
//
// MessageText:
//
//  v2 205 v24%0
//
#define AICF_WING_AAA_SPOT_2             0x00000854L

//
// v2 plane callsign with number.
//
// 509 <guns below us at>
//
// v24 Clock position
// v2 509 v24
//
// MessageId: AICF_WING_AAA_SPOT_3
//
// MessageText:
//
//  v2 509 v24%0
//
#define AICF_WING_AAA_SPOT_3             0x00000855L

//
// v2 plane callsign with number.
//
// 510 <Triple A fire spotted>
// v2 510
//
// MessageId: AICF_WING_AAA_SPOT_4
//
// MessageText:
//
//  v2 510%0
//
#define AICF_WING_AAA_SPOT_4             0x00000856L

//
// 707 <single engine>
// 707
//
// MessageId: AIF_SINGLE_ENGINE
//
// MessageText:
//
//  707%0
//
#define AIF_SINGLE_ENGINE                0x00000857L

//
// 708 <gadget bent>
// 708
//
// MessageId: AIF_GADGET_BENT
//
// MessageText:
//
//  708%0
//
#define AIF_GADGET_BENT                  0x00000858L

//
// 709 <bingo fuel>
// 709
//
// MessageId: AIF_BINGO_FUEL
//
// MessageText:
//
//  709%0
//
#define AIF_BINGO_FUEL                   0x00000859L

//
// 710 <hydraulics bent>
// 710
//
// MessageId: AIF_HYDRAULICS_BENT
//
// MessageText:
//
//  710%0
//
#define AIF_HYDRAULICS_BENT              0x0000085AL

//
// 303 <Laser on>
// Not Translated
//
// MessageId: AICF_WSO_LASER_ON
//
// MessageText:
//
//  303%0
//
#define AICF_WSO_LASER_ON                0x0000085BL

//
// 304 <Laser off>
// Not Translated
//
// MessageId: AICF_WSO_LASER_OFF
//
// MessageText:
//
//  304%0
//
#define AICF_WSO_LASER_OFF               0x0000085CL

//
// 305 <Losing lase, watch your steering.>
// Not Translated
//
// MessageId: AICF_WSO_LOOSING_LASE
//
// MessageText:
//
//  305%0
//
#define AICF_WSO_LOOSING_LASE            0x0000085DL

//
// 306 <The pod is masked, lost designation.>
// Not Translated
//
// MessageId: AICF_WSO_POD_MASKED
//
// MessageText:
//
//  306%0
//
#define AICF_WSO_POD_MASKED              0x0000085EL

//
// 245 <Music On>
// Not Translated
//
// MessageId: AICF_WSO_MUSIC_ON
//
// MessageText:
//
//  245%0
//
#define AICF_WSO_MUSIC_ON                0x0000085FL

//
// 246 <Music Off>
// Not Translated
//
// MessageId: AICF_WSO_MUSIC_OFF
//
// MessageText:
//
//  246%0
//
#define AICF_WSO_MUSIC_OFF               0x00000860L

//
// 309 <Two, tally your bandit, engaged.>
// Not Translated
//
// MessageId: AICF_ATTACK_MY_TARG_POS
//
// MessageText:
//
//  309%0
//
#define AICF_ATTACK_MY_TARG_POS          0x00000861L

//
// 310 <Two, no joy, unable to comply.>
// Not Translated
//
// MessageId: AICF_ATTACK_MY_TARG_NEG
//
// MessageText:
//
//  310%0
//
#define AICF_ATTACK_MY_TARG_NEG          0x00000862L

//
// 311 <Two, visual on lead, on my way.>
// Not Translated
//
// MessageId: AICF_HELP_ME_POS_1
//
// MessageText:
//
//  311%0
//
#define AICF_HELP_ME_POS_1               0x00000863L

//
// 312 <Two, visual on lead, tally bandit, 10 seconds.>
// Not Translated
//
// MessageId: AICF_HELP_ME_POS_2
//
// MessageText:
//
//  312%0
//
#define AICF_HELP_ME_POS_2               0x00000864L

//
// 313 <Two, blind, unable to comply.>
// Not Translated
//
// MessageId: AICF_HELP_ME_NEG_1
//
// MessageText:
//
//  313%0
//
#define AICF_HELP_ME_NEG_1               0x00000865L

//
// 314 <Two is engaged defensive at this time!>
// Not Translated
//
// MessageId: AICF_HELP_ME_NEG_2
//
// MessageText:
//
//  314%0
//
#define AICF_HELP_ME_NEG_2               0x00000866L

//
// v3 position in group
//
// 634 <negative>
// v3 634
//
// MessageId: AICF_BASIC_NEG_1
//
// MessageText:
//
//  v3 634%0
//
#define AICF_BASIC_NEG_1                 0x00000867L

//
// v3 position in group
//
// 635 <unable to comply> message
// v3 635
//
// MessageId: AICF_BASIC_NEG_2
//
// MessageText:
//
//  v3 635%0
//
#define AICF_BASIC_NEG_2                 0x00000868L

//
// v3 Number of plane in group
// v3
//
// MessageId: AICF_BASIC_ACK
//
// MessageText:
//
//  v3%0
//
#define AICF_BASIC_ACK                   0x00000869L

//
// v3 Number of plane in group
//
// 631 <wilco>
// v3 631
//
// MessageId: AICF_BASIC_ACK_2
//
// MessageText:
//
//  v3 631%0
//
#define AICF_BASIC_ACK_2                 0x0000086AL

//
// 753 <Two copy>
// 753
//
// MessageId: AICF_TWO_COPY_ACK
//
// MessageText:
//
//  753%0
//
#define AICF_TWO_COPY_ACK                0x0000086BL

//
// 530 <Two returning to formation.>
// Not Translated
//
// MessageId: AICF_TWO_RETURN_REFORM
//
// MessageText:
//
//  530%0
//
#define AICF_TWO_RETURN_REFORM           0x0000086CL

//
// v3 wingman number
//
// 588 <sorted>
//
// r2 cardinal2 heading <àern> or <leading/trailing>
//
// r4 bandit type
// v3 588 r2  r4
//
// MessageId: AICF_SORTED
//
// MessageText:
//
//  v3 588 r2  r4%0
//
#define AICF_SORTED                      0x0000086DL

//
// v3 wingman number
//
// 588 <sorted>
//
// 59 <middle>
//
// r3 plane number in middle group
//
// r4 bandit type
// v3 588 59 r3  r4
//
// MessageId: AICF_SORTED_MIDDLE
//
// MessageText:
//
//  v3 588 59 r3  r4%0
//
#define AICF_SORTED_MIDDLE               0x0000086EL

//
// v27 Player callsign
//
// v1 AWACS callsign
//
// 509 <copy>
//
// r4 Tanker callsign
//
// 504 <bearing>
//
// v7 bearing to Tanker (000-359)
//
// v10 range to Tanker
// Not Translated
//
// MessageId: AICF_AWACS_CLOSEST_TANKER
//
// MessageText:
//
//  v27 v1 509 r4 504 v7 v10%0
//
#define AICF_AWACS_CLOSEST_TANKER        0x0000086FL

//
// v3 position in group
//
// 501 <clean>
// Not Translated
//
// MessageId: AICF_WSO_CLEAN_MSG
//
// MessageText:
//
//  v3 501%0
//
#define AICF_WSO_CLEAN_MSG               0x00000870L

//
// 223 <He's extending>
// Not Translated
//
// MessageId: AICF_ENEMY_EXTENDING
//
// MessageText:
//
//  223%0
//
#define AICF_ENEMY_EXTENDING             0x00000871L

//
// 224 <He's turning back into us.>
// Not Translated
//
// MessageId: AICF_ENEMY_TURNING_BACK_TO_US
//
// MessageText:
//
//  224%0
//
#define AICF_ENEMY_TURNING_BACK_TO_US    0x00000872L

//
// 225 <He's breaking right.>
// Not Translated
//
// MessageId: AICF_ENEMY_BREAKING_RIGHT
//
// MessageText:
//
//  225%0
//
#define AICF_ENEMY_BREAKING_RIGHT        0x00000873L

//
// 226 <He's breaking left.>
// Not Translated
//
// MessageId: AICF_ENEMY_BREAKING_LEFT
//
// MessageText:
//
//  226%0
//
#define AICF_ENEMY_BREAKING_LEFT         0x00000874L

//
// 227 <He's converting to our six!>
// Not Translated
//
// MessageId: AICF_ENEMY_CONVERT_SIX
//
// MessageText:
//
//  227%0
//
#define AICF_ENEMY_CONVERT_SIX           0x00000875L

//
// 228 <He's on our six!>
// Not Translated
//
// MessageId: AICF_ENEMY_ON_SIX
//
// MessageText:
//
//  228%0
//
#define AICF_ENEMY_ON_SIX                0x00000876L

//
// 229 <Get this guy off of us.>
// Not Translated
//
// MessageId: AICF_GET_ENEMY_OFF_US
//
// MessageText:
//
//  229%0
//
#define AICF_GET_ENEMY_OFF_US            0x00000877L

//
// 238 <Shake this guy!>
// Not Translated
//
// MessageId: AICF_SHAKE_THIS_GUY
//
// MessageText:
//
//  238%0
//
#define AICF_SHAKE_THIS_GUY              0x00000878L

//
// 239 <C'mon, get this guy off our ass!>
// Not Translated
//
// MessageId: AICF_GET_GUY_OFF_ASS
//
// MessageText:
//
//  239%0
//
#define AICF_GET_GUY_OFF_ASS             0x00000879L

//
// 230 <This guy is good!>
// Not Translated
//
// MessageId: AICF_ENEMY_GOOD
//
// MessageText:
//
//  230%0
//
#define AICF_ENEMY_GOOD                  0x0000087AL

//
// 232 <He's got a missile off heading our way!>
// Not Translated
//
// MessageId: AICF_ENEMY_MISSILE_OFF
//
// MessageText:
//
//  232%0
//
#define AICF_ENEMY_MISSILE_OFF           0x0000087BL

//
// 233 <He's going vertical!>
// Not Translated
//
// MessageId: AICF_ENEMY_VERTICLE
//
// MessageText:
//
//  233%0
//
#define AICF_ENEMY_VERTICLE              0x0000087CL

//
// 234 <He's heading for the weeds!>
// Not Translated
//
// MessageId: AICF_ENEMY_WEEDS
//
// MessageText:
//
//  234%0
//
#define AICF_ENEMY_WEEDS                 0x0000087DL

//
// 235 <He's disengaging to the>
//
// r2 Cardinal direction, intense
// Not Translated
//
// MessageId: AICF_ENEMY_DISENGAGE
//
// MessageText:
//
//  235 r2%0
//
#define AICF_ENEMY_DISENGAGE             0x0000087EL

//
// r1 <Bandit> or <Enemy Helicopter>
//
// v24 clock position high / low
// Not Translated
//
// MessageId: AICF_BANDIT_CALL
//
// MessageText:
//
//  r1 v24%0
//
#define AICF_BANDIT_CALL                 0x0000087FL

//
// v32 Wingman number intense
//
// 522 <break right>
// v32 522
//
// MessageId: AICF_WING_BREAK_RIGHT
//
// MessageText:
//
//  v32 522%0
//
#define AICF_WING_BREAK_RIGHT            0x00000880L

//
// v32 Wingman number intense
//
// 523 <break left>
// v32 523
//
// MessageId: AICF_WING_BREAK_LEFT
//
// MessageText:
//
//  v32 523%0
//
#define AICF_WING_BREAK_LEFT             0x00000881L

//
// v2 Full callsign
//
// v10 range
//
// v9 cardinal 2 <bullseye>
//
// v11 altitude
//
// v8 cardinal 
// Not Translated
//
// MessageId: AICF_DO_RADAR_BULLSEYE
//
// MessageText:
//
//  v2 v20 v10 v9 598 v11 603 v8%0
//
#define AICF_DO_RADAR_BULLSEYE           0x00000882L

//
// v2 Full callsign
//
// v7 bearing
//
// v10 range
//
// v40 altitude2
//
// v15 aspect
// Not Translated
//
// MessageId: AICF_DO_RADAR_BRA
//
// MessageText:
//
//  v2 v20 608 v7 v10 v40 v15%0
//
#define AICF_DO_RADAR_BRA                0x00000883L

//
// v2 Pilot callsign
//
// v14 <bandit bandit> or <chopper>
//
// v24 o'clock position of enemy
//
// v15 closing/flanking/going away
// Not Translated
//
// MessageId: AICF_VISUAL_CONTACT_2
//
// MessageText:
//
//  v2 v41 v24 v15%0
//
#define AICF_VISUAL_CONTACT_2            0x00000884L

//
// v28 tanker callsign
//
// v2 flight callsign
//
// 658 <switches safe, nose cold, looking for,
//
// r3 tanker gives
// Not Translated
//
// MessageId: AICF_TANKER_LOOKING_FOR
//
// MessageText:
//
//  v28 v2 658 r3%0
//
#define AICF_TANKER_LOOKING_FOR          0x00000885L

//
// v26 Flight callsign
//
// 401 <cleared for takeoff, contact Departure>
// v26 401
//
// MessageId: AICF_CLEARED_FOR_TAKEOFF_DEPARTURE
//
// MessageText:
//
//  v26 401%0
//
#define AICF_CLEARED_FOR_TAKEOFF_DEPARTURE 0x00000886L

//
// v5 plane callsign and number intense.
//
// 514 <Mud Launch>
//
// v37 clock position intense
// v5 514 v37
//
// MessageId: AICF_WING_SAM_SPOT_4
//
// MessageText:
//
//  v5 514 v37%0
//
#define AICF_WING_SAM_SPOT_4             0x00000887L

//
// v6 Wingman callsign
//
// 517 <is engaged defensive ALAMO>
// v6 517
//
// MessageId: AICF_WING_DEFENSIVE_ALAMO
//
// MessageText:
//
//  v6 517%0
//
#define AICF_WING_DEFENSIVE_ALAMO        0x00000888L

//
// v25 Flight callsign
//
// 679 <rolling>
// v25 679
//
// MessageId: AICF_ROLLING
//
// MessageText:
//
//  v25 679%0
//
#define AICF_ROLLING                     0x00000889L

//
// v2 flight callsign
//
// 577 <Magnum!>
// v2 577
//
// MessageId: AICF_WING_MAGNUM
//
// MessageText:
//
//  v2 577%0
//
#define AICF_WING_MAGNUM                 0x0000088AL

//
// v2 wingman callsign
//
// 542 <hits in target area, additional targets remaining>
// v2 542
//
// MessageId: AICF_WING_HITS_IN_AREA
//
// MessageText:
//
//  v2 542%0
//
#define AICF_WING_HITS_IN_AREA           0x0000088BL

//
// v3 Number of plane in group
//
// 632 <roger>
// v3 632
//
// MessageId: AICF_ROGER
//
// MessageText:
//
//  v3 632%0
//
#define AICF_ROGER                       0x0000088CL

//
// v3 Number of plane in group
//
// 633 <copy>
// v3 633
//
// MessageId: AICF_COPY
//
// MessageText:
//
//  v3 633%0
//
#define AICF_COPY                        0x0000088DL

//
// v3 Number of plane in group
//
// 683 < tally your bandit, engaged.>
// v3 683
//
// MessageId: AICF_ATTACK_MY_TARG_POS_2
//
// MessageText:
//
//  v3 683%0
//
#define AICF_ATTACK_MY_TARG_POS_2        0x0000088EL

//
// v3 Number of plane in group
//
// 684 <no joy, unable to comply.>
// v3 684
//
// MessageId: AICF_ATTACK_MY_TARG_NEG_2
//
// MessageText:
//
//  v3 684%0
//
#define AICF_ATTACK_MY_TARG_NEG_2        0x0000088FL

//
// v3 Number of plane in group
//
// 685 <Two, visual on lead, on my way.>
// v3 685
//
// MessageId: AICF_HELP_ME_POS_1_A
//
// MessageText:
//
//  v3 685%0
//
#define AICF_HELP_ME_POS_1_A             0x00000890L

//
// v3 Number of plane in group
//
// 686 <Two, visual on lead, tally bandit, 10 seconds.>
// v3 686
//
// MessageId: AICF_HELP_ME_POS_2_A
//
// MessageText:
//
//  v3 686%0
//
#define AICF_HELP_ME_POS_2_A             0x00000891L

//
// v3 Number of plane in group
//
// 687 <Two, blind, unable to comply.>
// v3 687
//
// MessageId: AICF_HELP_ME_NEG_1_A
//
// MessageText:
//
//  v3 687%0
//
#define AICF_HELP_ME_NEG_1_A             0x00000892L

//
// v3 Number of plane in group
//
// 688 <Two is engaged defensive at this time!>
// v3 688
//
// MessageId: AICF_HELP_ME_NEG_2_A
//
// MessageText:
//
//  v3 688%0
//
#define AICF_HELP_ME_NEG_2_A             0x00000893L

//
// v27 Player callsign
//
// 372 <tanker bearing>
//
// v7 bearing to Tanker (000-359)
//
// v10 range to Tanker
//
// v40 Altitude <thousand>
// v27 372 v7 v10 v40
//
// MessageId: AICF_TANKER_BEARING
//
// MessageText:
//
//  v27 372 v7 v10 v40%0
//
#define AICF_TANKER_BEARING              0x00000894L

//
// v35 Ship callsign <Strike/Marshall>
//
// v2 callsign or modex
//
// 731 <inbound Mother's>
//
// v7 bearing from mother
//
// v10 range
//
// v39 altitude 3
//
// v42 fuel state
// v35 v2 731 v7 v10 v39 v42
//
// MessageId: AICF_TELL_STRIKE_INBOUND
//
// MessageText:
//
//  v35 v2 731 v7 v10 v39 v42%0
//
#define AICF_TELL_STRIKE_INBOUND         0x00000895L

//
// v28 Callsign or modex
//
// 294 <Strike, cleared inbound>
// v28 294
//
// MessageId: AICF_STRIKE_CLEARED_IN
//
// MessageText:
//
//  v28 294%0
//
#define AICF_STRIKE_CLEARED_IN           0x00000896L

//
// v28 callsign or modex
//
// 295 <switch Marshal>
// v28 295
//
// MessageId: AICF_SWITCH_MARSHAL
//
// MessageText:
//
//  v28 295%0
//
#define AICF_SWITCH_MARSHAL              0x00000897L

//
// v28 Callsign or modex
//
// 306 <Marshal, Mother's weather, ceiling>
//
// r3 ceiling
//
// 304 <visibility>
//
// v10 range
// v28 306 r3 304 v10
//
// MessageId: AICF_MARSHAL_CHECK
//
// MessageText:
//
//  v28 306 r3 304 v10%0
//
#define AICF_MARSHAL_CHECK               0x00000898L

//
// 307 <Case three recoveries.  Marshal on the>
//
// v7 bearing
//
// 308 <radial>
//
// v10 range
//
// v39 altitude 3
//
// 310 <expected approach time>
//
// r1 time
//
// 311 <Time now>
//
// r2 current time
// 307 v7 308 v10 v39 310 r1 311 r2
//
// MessageId: AICF_CASE_III
//
// MessageText:
//
//  307 v7 308 v10 v39 310 r1 311 r2%0
//
#define AICF_CASE_III                    0x00000899L

//
// v2 callsign or modex
//
// 732 <established>
//
// v39 altitude 3
//
// v42 fuel state
// v2 732 v39 v42
//
// MessageId: AICF_ESTABLISHED_MARSHAL
//
// MessageText:
//
//  v2 732 v39 v42%0
//
#define AICF_ESTABLISHED_MARSHAL         0x0000089AL

//
// v2 callsign or modex
//
// 733 <commencing>
//
// v42 fuel state
// v2 733 v42
//
// MessageId: AICF_COMMENCING_PUSH
//
// MessageText:
//
//  v2 733 v42%0
//
#define AICF_COMMENCING_PUSH             0x0000089BL

//
// v2 callsign or modex
//
// 734 <platform>
//
// v42 fuel state
// v2 734 v42
//
// MessageId: AICF_PLATFORM
//
// MessageText:
//
//  v2 734 v42%0
//
#define AICF_PLATFORM                    0x0000089CL

//
// v2 callsign or modex
//
// v10 DME range
//
// v42 fuel state
// v2 v10 v42
//
// MessageId: AICF_LANDING_DISTANCE
//
// MessageText:
//
//  v2 v10 v42%0
//
#define AICF_LANDING_DISTANCE            0x0000089DL

//
// v28 Callsign or modex
//
// 300 <five miles, A C L S lock-on, call your needles>
// Not Translated
//
// MessageId: AICF_FINAL_ACLS_CONTACT
//
// MessageText:
//
//  v28 300%0
//
#define AICF_FINAL_ACLS_CONTACT          0x0000089EL

//
// v28 Callsign or modex
//
// 301 <concur>
// v28 301
//
// MessageId: AICF_CONCUR
//
// MessageText:
//
//  v28 301%0
//
#define AICF_CONCUR                      0x0000089FL

//
// v28 Callsign or modex
//
// 303 <three quarters of a mile, call the ball>
// v28 303
//
// MessageId: AICF_FINAL_CALL_BALL
//
// MessageText:
//
//  v28 303%0
//
#define AICF_FINAL_CALL_BALL             0x000008A0L

//
// v2 callsign or modex
//
// 736 <hornet ball>
//
// v42 fuel state
// v2 736 v42
//
// MessageId: AICF_HORNET_BALL
//
// MessageText:
//
//  v2 736 v42%0
//
#define AICF_HORNET_BALL                 0x000008A1L

//
// v2 callsign or modex
//
// 737 <super hornet ball>
//
// v42 fuel state
// v2 737 v42
//
// MessageId: AICF_S_HORNET_BALL
//
// MessageText:
//
//  v2 737 v42%0
//
#define AICF_S_HORNET_BALL               0x000008A2L

//
// v2 callsign or modex
//
// 738 <prowler ball>
//
// v42 fuel state
// v2 738 v42
//
// MessageId: AICF_PROWLER_BALL
//
// MessageText:
//
//  v2 738 v42%0
//
#define AICF_PROWLER_BALL                0x000008A3L

//
// v2 callsign or modex
//
// 739 <tomcat ball>
//
// v42 fuel state
// v2 739 v42
//
// MessageId: AICF_TOMCAT_BALL
//
// MessageText:
//
//  v2 739 v42%0
//
#define AICF_TOMCAT_BALL                 0x000008A4L

//
// v2 callsign or modex
//
// 740 <viking ball>
//
// v42 fuel state
// v2 740 v42
//
// MessageId: AICF_VIKING_BALL
//
// MessageText:
//
//  v2 740 v42%0
//
#define AICF_VIKING_BALL                 0x000008A5L

//
// v2 callsign or modex
//
// 736 <hawkeye ball>
//
// v42 fuel state
// v2 736 v42
//
// MessageId: AICF_HAWKEYE_BALL
//
// MessageText:
//
//  v2 736 v42%0
//
#define AICF_HAWKEYE_BALL                0x000008A6L

//
// v2 callsign or modex
//
// 741 <clara>
//
// v42 fuel state
// v2 741 v42
//
// MessageId: AICF_CLARA
//
// MessageText:
//
//  v2 741 v42%0
//
#define AICF_CLARA                       0x000008A7L

//
// v2 callsign or modex
//
// 742 <abeam>
//
// v42 fuel state
// v2 742 v42
//
// MessageId: AICF_ABEAM
//
// MessageText:
//
//  v2 742 v42%0
//
#define AICF_ABEAM                       0x000008A8L

//
// v2 callsign or modex
//
// 743 <on glideslope>
// v2 743
//
// MessageId: AICF_ACLS_ON_GLIDESLOPE
//
// MessageText:
//
//  v2 743%0
//
#define AICF_ACLS_ON_GLIDESLOPE          0x000008A9L

//
// v2 callsign or modex
//
// 744 <fly up>
// v2 744
//
// MessageId: AICF_ACLS_FLY_UP
//
// MessageText:
//
//  v2 744%0
//
#define AICF_ACLS_FLY_UP                 0x000008AAL

//
// v2 callsign or modex
//
// 745 fly down
// v2 745
//
// MessageId: AICF_ACLS_FLY_DOWN
//
// MessageText:
//
//  v2 745%0
//
#define AICF_ACLS_FLY_DOWN               0x000008ABL

//
// 746 <on centerline>
// 746
//
// MessageId: AICF_ACLS_ON_CENTERLINE
//
// MessageText:
//
//  746%0
//
#define AICF_ACLS_ON_CENTERLINE          0x000008ACL

//
// 748 <fly left>
// 748
//
// MessageId: AICF_ACLS_FLY_LEFT
//
// MessageText:
//
//  748%0
//
#define AICF_ACLS_FLY_LEFT               0x000008ADL

//
// 747 <fly right>
// 747
//
// MessageId: AICF_ACLS_FLY_RIGHT
//
// MessageText:
//
//  747%0
//
#define AICF_ACLS_FLY_RIGHT              0x000008AEL

//
// v28 Callsign or modex
//
// 313 <what are you doing?>
// v28 313
//
// MessageId: AICF_WHAT_DOING
//
// MessageText:
//
//  v28 313%0
//
#define AICF_WHAT_DOING                  0x000008AFL

//
// v28 Callsign or modex
//
// 314 <remain in the marshal stack>
// v28 314
//
// MessageId: AICF_REMAIN_MARSHAL_1
//
// MessageText:
//
//  v28 314%0
//
#define AICF_REMAIN_MARSHAL_1            0x000008B0L

//
// v28 Callsign or modex
//
// 315 <return to the marshal stack>
// v28 315
//
// MessageId: AICF_REMAIN_MARSHAL_2
//
// MessageText:
//
//  v28 315%0
//
#define AICF_REMAIN_MARSHAL_2            0x000008B1L

//
// v28 Callsign or modex
//
// 312 <commence now>
// v28 312
//
// MessageId: AICF_COMMENCE_NOW
//
// MessageText:
//
//  v28 312%0
//
#define AICF_COMMENCE_NOW                0x000008B2L

//
// 400 <You're a little high>
// 400
//
// MessageId: AICF_LITTLE_HIGH
//
// MessageText:
//
//  400%0
//
#define AICF_LITTLE_HIGH                 0x000008B3L

//
// 401 <You're high>
// 401
//
// MessageId: AICF_U_HIGH
//
// MessageText:
//
//  401%0
//
#define AICF_U_HIGH                      0x000008B4L

//
// 402 <You're a little low>
// 402
//
// MessageId: AICF_LITTLE_LOW
//
// MessageText:
//
//  402%0
//
#define AICF_LITTLE_LOW                  0x000008B5L

//
// 403 <You're low>
// 403
//
// MessageId: AICF_U_LOW
//
// MessageText:
//
//  403%0
//
#define AICF_U_LOW                       0x000008B6L

//
// 404 <You're going high>
// 404
//
// MessageId: AICF_GOING_HIGH
//
// MessageText:
//
//  404%0
//
#define AICF_GOING_HIGH                  0x000008B7L

//
// 405 <You're going low>
// 405
//
// MessageId: AICF_GOING_LOW
//
// MessageText:
//
//  405%0
//
#define AICF_GOING_LOW                   0x000008B8L

//
// 406 <You're on centerline>
// 406
//
// MessageId: AICF_U_ON_CENTERLINE
//
// MessageText:
//
//  406%0
//
#define AICF_U_ON_CENTERLINE             0x000008B9L

//
// 407 <You're on glideslope>
// 407
//
// MessageId: AICF_U_ON_GLIDESLOPE
//
// MessageText:
//
//  407%0
//
#define AICF_U_ON_GLIDESLOPE             0x000008BAL

//
// 408 <You're lined up left>
// 408
//
// MessageId: AICF_U_LINED_LEFT
//
// MessageText:
//
//  408%0
//
#define AICF_U_LINED_LEFT                0x000008BBL

//
// 409 <You're lined up right>
// 409
//
// MessageId: AICF_U_LINED_RIGHT
//
// MessageText:
//
//  409%0
//
#define AICF_U_LINED_RIGHT               0x000008BCL

//
// 410 <You're drifing left>
// 410
//
// MessageId: AICF_U_DRIFTING_LEFT
//
// MessageText:
//
//  410%0
//
#define AICF_U_DRIFTING_LEFT             0x000008BDL

//
// 411 You're drifing right>
// 411
//
// MessageId: AICF_U_DRIFTING_RIGHT
//
// MessageText:
//
//  411%0
//
#define AICF_U_DRIFTING_RIGHT            0x000008BEL

//
// 412 <You're fast>
// 412
//
// MessageId: AICF_U_FAST
//
// MessageText:
//
//  412%0
//
#define AICF_U_FAST                      0x000008BFL

//
// 413 <You're slow>
// 412
//
// MessageId: AICF_U_SLOW
//
// MessageText:
//
//  412%0
//
#define AICF_U_SLOW                      0x000008C0L

//
// 414 <Roger ball>
// 414
//
// MessageId: AICF_ROGER_BALL
//
// MessageText:
//
//  414%0
//
#define AICF_ROGER_BALL                  0x000008C1L

//
// 415 <Keep coming>
// 415
//
// MessageId: AICF_KEEP_COMING
//
// MessageText:
//
//  415%0
//
#define AICF_KEEP_COMING                 0x000008C2L

//
// 416 <Paddles contact>
// 416
//
// MessageId: AICF_PADDLES_CONTACT
//
// MessageText:
//
//  416%0
//
#define AICF_PADDLES_CONTACT             0x000008C3L

//
// 417 <The deck is moving up a little>
// 417
//
// MessageId: AICF_DECK_LITTLE_UP
//
// MessageText:
//
//  417%0
//
#define AICF_DECK_LITTLE_UP              0x000008C4L

//
// 418 <The deck is moving up>
// 418
//
// MessageId: AICF_DECK_UP
//
// MessageText:
//
//  418%0
//
#define AICF_DECK_UP                     0x000008C5L

//
// 419 <The deck is moving down a little>
// 419
//
// MessageId: AICF_DECK_LITTLE_DOWN
//
// MessageText:
//
//  419%0
//
#define AICF_DECK_LITTLE_DOWN            0x000008C6L

//
// 420 <The deck is moving down>
// 420
//
// MessageId: AICF_DECK_DOWN
//
// MessageText:
//
//  420%0
//
#define AICF_DECK_DOWN                   0x000008C7L

//
// 421 <The deck is steady>
// 421
//
// MessageId: AICF_DECK_STEADY
//
// MessageText:
//
//  421%0
//
#define AICF_DECK_STEADY                 0x000008C8L

//
// 422 <Winds are slighty starboard>
// 422
//
// MessageId: AICF_WIND_S_STARBOARD
//
// MessageText:
//
//  422%0
//
#define AICF_WIND_S_STARBOARD            0x000008C9L

//
// 423 <Winds are starboard>
// 423
//
// MessageId: AICF_WIND_STARBOARD
//
// MessageText:
//
//  423%0
//
#define AICF_WIND_STARBOARD              0x000008CAL

//
// 424 <Winds are slightly port>
// 424
//
// MessageId: AICF_WIND_S_PORT
//
// MessageText:
//
//  424%0
//
#define AICF_WIND_S_PORT                 0x000008CBL

//
// 425 <Winds are port>
// 425
//
// MessageId: AICF_WIND_PORT
//
// MessageText:
//
//  425%0
//
#define AICF_WIND_PORT                   0x000008CCL

//
// 426 <Winds are axial>
// 426
//
// MessageId: AICF_WIND_AXIAL
//
// MessageText:
//
//  426%0
//
#define AICF_WIND_AXIAL                  0x000008CDL

//
// 427 <You're underpowered>
// 427
//
// MessageId: AICF_U_UNDERPOWERED
//
// MessageText:
//
//  427%0
//
#define AICF_U_UNDERPOWERED              0x000008CEL

//
// 428 <You're overpowered>
// 428
//
// MessageId: AICF_U_OVERPOWERED
//
// MessageText:
//
//  428%0
//
#define AICF_U_OVERPOWERED               0x000008CFL

//
// 429 <Ship's in a starboard turn>
// 429
//
// MessageId: AICF_SHIP_STARBOARD_TURN
//
// MessageText:
//
//  429%0
//
#define AICF_SHIP_STARBOARD_TURN         0x000008D0L

//
// 430 <Ship's in a port turn>
// 430
//
// MessageId: AICF_SHIP_PORT_TURN
//
// MessageText:
//
//  430%0
//
#define AICF_SHIP_PORT_TURN              0x000008D1L

//
// 431 <Keep your turn in>
// 431
//
// MessageId: AICF_KEEP_TURN_IN
//
// MessageText:
//
//  431%0
//
#define AICF_KEEP_TURN_IN                0x000008D2L

//
// 432 <Check your lineup>
// 432
//
// MessageId: AICF_CHECK_LINEUP
//
// MessageText:
//
//  432%0
//
#define AICF_CHECK_LINEUP                0x000008D3L

//
// 433 <Back to the right>
// 433
//
// MessageId: AICF_BACK_RIGHT
//
// MessageText:
//
//  433%0
//
#define AICF_BACK_RIGHT                  0x000008D4L

//
// 434 <Back to the left>
// 434
//
// MessageId: AICF_BACK_LEFT
//
// MessageText:
//
//  434%0
//
#define AICF_BACK_LEFT                   0x000008D5L

//
// 435 <Don't settle>
// 435
//
// MessageId: AICF_DO_NOT_SETTLE
//
// MessageText:
//
//  435%0
//
#define AICF_DO_NOT_SETTLE               0x000008D6L

//
// 436 <Don't go low>
// 436
//
// MessageId: AICF_DO_NOT_GO_LOW
//
// MessageText:
//
//  436%0
//
#define AICF_DO_NOT_GO_LOW               0x000008D7L

//
// 437 <Don't climb>
// 437
//
// MessageId: AICF_DO_NOT_CLIMB
//
// MessageText:
//
//  437%0
//
#define AICF_DO_NOT_CLIMB                0x000008D8L

//
// 438 <Don't go high>
// 438
//
// MessageId: AICF_DO_NOT_GO_HIGH
//
// MessageText:
//
//  438%0
//
#define AICF_DO_NOT_GO_HIGH              0x000008D9L

//
// 439 <Don't go any lower>
// 439
//
// MessageId: AICF_NO_LOWER
//
// MessageText:
//
//  439%0
//
#define AICF_NO_LOWER                    0x000008DAL

//
// 440 <Don't go any higher>
// 440
//
// MessageId: AICF_NO_HIGHER
//
// MessageText:
//
//  440%0
//
#define AICF_NO_HIGHER                   0x000008DBL

//
// 441 <Power back on>
// 441
//
// MessageId: AICF_POWER_ON
//
// MessageText:
//
//  441%0
//
#define AICF_POWER_ON                    0x000008DCL

//
// 442 <Don't settle through it.
// 442
//
// MessageId: AICF_NO_SETTLE_THROUGH
//
// MessageText:
//
//  442%0
//
#define AICF_NO_SETTLE_THROUGH           0x000008DDL

//
// 443 <Hold what you've got>
// 443
//
// MessageId: AICF_HOLD_U_GOT
//
// MessageText:
//
//  443%0
//
#define AICF_HOLD_U_GOT                  0x000008DEL

//
// 444 <Fly the ball>
// 444
//
// MessageId: AICF_FLY_BALL
//
// MessageText:
//
//  444%0
//
#define AICF_FLY_BALL                    0x000008DFL

//
// 445 <Easy with it>
// 445
//
// MessageId: AICF_EASY
//
// MessageText:
//
//  445%0
//
#define AICF_EASY                        0x000008E0L

//
// 446 <Easy with your wings>
// 446
//
// MessageId: AICF_EASY_WINGS
//
// MessageText:
//
//  446%0
//
#define AICF_EASY_WINGS                  0x000008E1L

//
// 447 <A little power>
// 447
//
// MessageId: AICF_LITTLE_POWER
//
// MessageText:
//
//  447%0
//
#define AICF_LITTLE_POWER                0x000008E2L

//
// 448 <Power>
// 448
//
// MessageId: AICF_POWER
//
// MessageText:
//
//  448%0
//
#define AICF_POWER                       0x000008E3L

//
// 449 <Attitude>
// 449
//
// MessageId: AICF_ATTITUDE
//
// MessageText:
//
//  449%0
//
#define AICF_ATTITUDE                    0x000008E4L

//
// 450 <A little attitude>
// 450
//
// MessageId: AICF_LITTLE_ATTITUDE
//
// MessageText:
//
//  450%0
//
#define AICF_LITTLE_ATTITUDE             0x000008E5L

//
// 451 <A little right rudder>
// 451
//
// MessageId: AICF_LITTLE_R_RUDDER
//
// MessageText:
//
//  451%0
//
#define AICF_LITTLE_R_RUDDER             0x000008E6L

//
// 452 <A little left rudder>
// 452
//
// MessageId: AICF_LITTLE_L_RUDDER
//
// MessageText:
//
//  452%0
//
#define AICF_LITTLE_L_RUDDER             0x000008E7L

//
// 453 <Left rudder>
// 453
//
// MessageId: AICF_LEFT_RUDDER
//
// MessageText:
//
//  453%0
//
#define AICF_LEFT_RUDDER                 0x000008E8L

//
// 454 <Right rudder>
// 454
//
// MessageId: AICF_RIGHT_RUDDER
//
// MessageText:
//
//  454%0
//
#define AICF_RIGHT_RUDDER                0x000008E9L

//
// 455 <A little right for lineup>
// 455
//
// MessageId: AICF_LITTLE_R_LINEUP
//
// MessageText:
//
//  455%0
//
#define AICF_LITTLE_R_LINEUP             0x000008EAL

//
// 456 <Come a little left>
// 456
//
// MessageId: AICF_COME_LITTLE_LEFT
//
// MessageText:
//
//  456%0
//
#define AICF_COME_LITTLE_LEFT            0x000008EBL

//
// 457 <Right for lineup>
// 457
//
// MessageId: AICF_RIGHT_LINEUP
//
// MessageText:
//
//  457%0
//
#define AICF_RIGHT_LINEUP                0x000008ECL

//
// 458 <Come left>
// 458
//
// MessageId: AICF_COME_LEFT
//
// MessageText:
//
//  458%0
//
#define AICF_COME_LEFT                   0x000008EDL

//
// 459 <Waveoff>
// 459
//
// MessageId: AICF_WAVEOFF
//
// MessageText:
//
//  459%0
//
#define AICF_WAVEOFF                     0x000008EEL

//
// 460 <Waveoff, foul deck>
// 460
//
// MessageId: AICF_WAVEOFF_DECK
//
// MessageText:
//
//  460%0
//
#define AICF_WAVEOFF_DECK                0x000008EFL

//
// 461 <Cut>
// 461
//
// MessageId: AICF_CUT
//
// MessageText:
//
//  461%0
//
#define AICF_CUT                         0x000008F0L

//
// 462 <Drop your hook>
// 462
//
// MessageId: AICF_DROP_HOOK
//
// MessageText:
//
//  462%0
//
#define AICF_DROP_HOOK                   0x000008F1L

//
// 463 <Drop your gear>
// 463
//
// MessageId: AICF_DROP_GEAR
//
// MessageText:
//
//  463%0
//
#define AICF_DROP_GEAR                   0x000008F2L

//
// 464 <Drop your flaps>
// 464
//
// MessageId: AICF_DROP_FLAPS
//
// MessageText:
//
//  464%0
//
#define AICF_DROP_FLAPS                  0x000008F3L

//
// 465 <Level your wings>
// 465
//
// MessageId: AICF_LEVEL_WINGS
//
// MessageText:
//
//  465%0
//
#define AICF_LEVEL_WINGS                 0x000008F4L

//
// 466 <Power!>
// 466
//
// MessageId: AICF_POWER_INTENSE
//
// MessageText:
//
//  466%0
//
#define AICF_POWER_INTENSE               0x000008F5L

//
// 467 <Burner!>
// 467
//
// MessageId: AICF_BURNER
//
// MessageText:
//
//  467%0
//
#define AICF_BURNER                      0x000008F6L

//
// 468 <Attitude!>
// 468
//
// MessageId: AICF_ATTITUDE_INTENSE
//
// MessageText:
//
//  468%0
//
#define AICF_ATTITUDE_INTENSE            0x000008F7L

//
// 469 <Left rudder!>
// 469
//
// MessageId: AICF_LEFT_RUDDER_INTENSE
//
// MessageText:
//
//  469%0
//
#define AICF_LEFT_RUDDER_INTENSE         0x000008F8L

//
// 470 <Right rudder!>
// 470
//
// MessageId: AICF_RIGHT_RUDDER_INTENSE
//
// MessageText:
//
//  470%0
//
#define AICF_RIGHT_RUDDER_INTENSE        0x000008F9L

//
// 471 <Right for lineup!>
// 471
//
// MessageId: AICF_RIGHT_LINEUP_INTENSE
//
// MessageText:
//
//  471%0
//
#define AICF_RIGHT_LINEUP_INTENSE        0x000008FAL

//
// 472 <Come left!>
// 472
//
// MessageId: AICF_COME_LEFT_INTENSE
//
// MessageText:
//
//  472%0
//
#define AICF_COME_LEFT_INTENSE           0x000008FBL

//
// 473 <Bolter!>
// 473
//
// MessageId: AICF_BOLTER
//
// MessageText:
//
//  473%0
//
#define AICF_BOLTER                      0x000008FCL

//
// 474 <Waveoff!>
// 474
//
// MessageId: AICF_WAVEOFF_INTENSE
//
// MessageText:
//
//  474%0
//
#define AICF_WAVEOFF_INTENSE             0x000008FDL

//
// 475 <Waveoff, Waveoff, Waveoff!>
// 475
//
// MessageId: AICF_WAVEOFF_3
//
// MessageText:
//
//  475%0
//
#define AICF_WAVEOFF_3                   0x000008FEL

//
// 476 <Drop your hook!>
// 476
//
// MessageId: AICF_DROP_HOOK_INTENSE
//
// MessageText:
//
//  476%0
//
#define AICF_DROP_HOOK_INTENSE           0x000008FFL

//
// 477 <Drop your gear!>
// 477
//
// MessageId: AICF_DROP_GEAR_INTENSE
//
// MessageText:
//
//  477%0
//
#define AICF_DROP_GEAR_INTENSE           0x00000900L

//
// 478 <Drop your flaps!>
// 478
//
// MessageId: AICF_DROP_FLAPS_INTENSE
//
// MessageText:
//
//  478%0
//
#define AICF_DROP_FLAPS_INTENSE          0x00000901L

//
// 479 <Level your wings!>
// 479
//
// MessageId: AICF_LEVEL_WINGS_INTENSE
//
// MessageText:
//
//  479%0
//
#define AICF_LEVEL_WINGS_INTENSE         0x00000902L

//
// 480 <Climb!>
// 480
//
// MessageId: AICF_CLIMB_INTENSE
//
// MessageText:
//
//  480%0
//
#define AICF_CLIMB_INTENSE               0x00000903L

//
// r1 AWACS or Strike callsign
//
// r2 Number of groups
//
// 600 <groups>
// r1 r2 600
//
// MessageId: AICF_GROUPS
//
// MessageText:
//
//  r1 r2 600%0
//
#define AICF_GROUPS                      0x00000904L

//
// r1 AWACS or Strike callsign
//
// r2 Number of groups
//
// 605 <singles>
// r1 r2 605
//
// MessageId: AICF_SINGLES
//
// MessageText:
//
//  r1 r2 605%0
//
#define AICF_SINGLES                     0x00000905L

//
// v10 range
//
// v43 cardinal bullseye
//
// v13 altitude
//
// 603 <tracking>
//
// v8 cardinal 
// v10 v43 v13 603 v8
//
// MessageId: AICF_DO_BULLSEYE
//
// MessageText:
//
//  v10 v43 v13 603 v8%0
//
#define AICF_DO_BULLSEYE                 0x00000906L

//
// v44 digital bullseye
//
// v10 range
//
// v40 altitude thousands
//
// 603 <tracking>
//
// v8 cardinal 
// v44 v10 v40 603 v8
//
// MessageId: AICF_DO_DIGIT_BULLSEYE
//
// MessageText:
//
//  v44 v10 v40 603 v8%0
//
#define AICF_DO_DIGIT_BULLSEYE           0x00000907L

//
// 608 <bra>
//
// v7 bearing
//
// v10 range
//
// v40 altitude2
//
// v15 aspect
// 608 v7 v10 v40 v15
//
// MessageId: AICF_DO_BRA
//
// MessageText:
//
//  608 v7 v10 v40 v15%0
//
#define AICF_DO_BRA                      0x00000908L

//
// v2 callsign
//
// 599 <lone group>
// v2 599
//
// MessageId: AICF_LONE_GROUP
//
// MessageText:
//
//  v2 599%0
//
#define AICF_LONE_GROUP                  0x00000909L

//
// v45 Order off numingroup
//
// 601 group
// v45 601
//
// MessageId: AICF_GROUP_INFO
//
// MessageText:
//
//  v45 601%0
//
#define AICF_GROUP_INFO                  0x0000090AL

//
// v2 callsign
//
// 601 group
// Not Translated
//
// MessageId: AICF_CS_GROUP_INFO
//
// MessageText:
//
//  v2 601%0
//
#define AICF_CS_GROUP_INFO               0x0000090BL

//
// v2 callsign
//
// 603 <popup group>
// v2 603
//
// MessageId: AICF_POPUP_GROUP
//
// MessageText:
//
//  v2 603%0
//
#define AICF_POPUP_GROUP                 0x0000090CL

//
// v2 callsign
//
// 604 <lone single>
// v2 604
//
// MessageId: AICF_LONE_SINGLE
//
// MessageText:
//
//  v2 604%0
//
#define AICF_LONE_SINGLE                 0x0000090DL

//
// v45 Order off numingroup
//
// 606 <single>
// Not Translated
//
// MessageId: AICF_SINGLE_INFO
//
// MessageText:
//
//  v45 606%0
//
#define AICF_SINGLE_INFO                 0x0000090EL

//
// v2 callsign
//
// 606 <single>
// Not Translated
//
// MessageId: AICF_CS_SINGLE_INFO
//
// MessageText:
//
//  v2 606%0
//
#define AICF_CS_SINGLE_INFO              0x0000090FL

//
// v2 callsign
//
// 607 <popup single>
// Not Translated
//
// MessageId: AICF_POPUP_SINGLE
//
// MessageText:
//
//  v2 607%0
//
#define AICF_POPUP_SINGLE                0x00000910L

//
// v2 callsign
//
// 596 <go broadcast>
// v2 596
//
// MessageId: AICF_WING_GO_BROADCAST
//
// MessageText:
//
//  v2 596%0
//
#define AICF_WING_GO_BROADCAST           0x00000911L

//
// v2 callsign
//
// 597 <go tactical>
// v2 597
//
// MessageId: AICF_WING_GO_TACTICAL
//
// MessageText:
//
//  v2 597%0
//
#define AICF_WING_GO_TACTICAL            0x00000912L

//
// v3 wingman number
//
// v42 fule state
// v3 v42
//
// MessageId: AICF_WING_FUEL_STATE
//
// MessageText:
//
//  v3 v42%0
//
#define AICF_WING_FUEL_STATE             0x00000913L

//
// v28 Pilot callsign off targetnum
//
// r2 variable something <bearing>
//
// v7 bearing
//
// v10 range
// v28 r2 v7 v10
//
// MessageId: AICF_ITEM_BEARING
//
// MessageText:
//
//  v28 r2 v7 v10%0
//
#define AICF_ITEM_BEARING                0x00000914L

//
// v28 Pilot callsign off targetnum
//
// r2 variable something <bearing>
//
// v7 bearing
//
// v10 range
//
// v40 altitude thousands
// v28 r2 v7 v10 v40
//
// MessageId: AICF_ITEM_BEARING_ALT
//
// MessageText:
//
//  v28 r2 v7 v10 v40%0
//
#define AICF_ITEM_BEARING_ALT            0x00000915L

//
// v3 wingman number
// v3
//
// MessageId: AICF_WING_POS_START
//
// MessageText:
//
//  v3%0
//
#define AICF_WING_POS_START              0x00000916L

//
// v10 range
//
// v43 cardinal bullseye
//
// v40 altitude thousands
//
// 603 <tracking>
//
// v8 cardinal 
// v10 v43 v40 603 v8
//
// MessageId: AICF_DO_POS_BULLSEYE
//
// MessageText:
//
//  v10 v43 v40 603 v8%0
//
#define AICF_DO_POS_BULLSEYE             0x00000917L

//
// v27 FAC callsign <pass callsign num for ground>
//
// v1 Pilot callsign
//
// r5 AG Missile string
//
// r6 Guided bomb string
//
// r8 Cluster bomb string
//
// r7 Dumb bomb string
//
// r9 Pod string
//
// 670 <on station>
// v27 v1 r5 r6 r8 r7 r9 670
//
// MessageId: AICF_FAC_CHECK_IN
//
// MessageText:
//
//  v27 v1 r5 r6 r8 r7 r9 670%0
//
#define AICF_FAC_CHECK_IN                0x00000918L

//
// v27 Player callsign
//
// 300 <copy>
// v27 300
//
// MessageId: AICF_FAC_COPY
//
// MessageText:
//
//  v27 300%0
//
#define AICF_FAC_COPY                    0x00000919L

//
// v27 Player callsign
//
// 301 <affirmative>
// v27 301
//
// MessageId: AICF_FAC_AFFIRM
//
// MessageText:
//
//  v27 301%0
//
#define AICF_FAC_AFFIRM                  0x0000091AL

//
// v27 Player callsign
//
// 302 <negative>
// v27 302
//
// MessageId: AICF_FAC_NEGATIVE
//
// MessageText:
//
//  v27 302%0
//
#define AICF_FAC_NEGATIVE                0x0000091BL

//
// v27 Player callsign
//
// 303 <standby>
// v27 303
//
// MessageId: AICF_FAC_STANDBY
//
// MessageText:
//
//  v27 303%0
//
#define AICF_FAC_STANDBY                 0x0000091CL

//
// v1 Pilot callsign
//
// 671 <established>
// v1 671
//
// MessageId: AICF_ESTABLISHED
//
// MessageText:
//
//  v1 671%0
//
#define AICF_ESTABLISHED                 0x0000091DL

//
// v27 Pilot callsign
//
// 304 <copy, standby for words>
// v27 304
//
// MessageId: AICF_STANDBY_FOR_WORDS
//
// MessageText:
//
//  v27 304%0
//
#define AICF_STANDBY_FOR_WORDS           0x0000091EL

//
// v27 Pilot callsign
//
// 305 <copy, standby for nine line brief>
// v27 305
//
// MessageId: AICF_STANDBY_FOR_NINE
//
// MessageText:
//
//  v27 305%0
//
#define AICF_STANDBY_FOR_NINE            0x0000091FL

//
// v27 Pilot callsign
//
// 306 <copy, standby for four line brief>
// v27 306
//
// MessageId: AICF_STANDBY_FOR_FOUR
//
// MessageText:
//
//  v27 306%0
//
#define AICF_STANDBY_FOR_FOUR            0x00000920L

//
// v27 Pilot callsign
//
// 333 <Check fire!  Check fire!  Your weaopns are landing within friendly lines!>
// v27 333
//
// MessageId: AICF_GROUND_FRIENDLY_FIRE_US_4
//
// MessageText:
//
//  v27 333%0
//
#define AICF_GROUND_FRIENDLY_FIRE_US_4   0x00000921L

//
// v27 Pilot callsign
//
// 335 <Abort your attack!  Friendlies are in that area!>
// v27 335
//
// MessageId: AICF_GROUND_FRIENDLY_FIRE_US_5
//
// MessageText:
//
//  v27 335%0
//
#define AICF_GROUND_FRIENDLY_FIRE_US_5   0x00000922L

//
// v27 Pilot callsign
//
// 336 <Cease fire!  Bittersweet, repeat Bittersweet>
// v27 336
//
// MessageId: AICF_GROUND_FRIENDLY_FIRE_US_6
//
// MessageText:
//
//  v27 336%0
//
#define AICF_GROUND_FRIENDLY_FIRE_US_6   0x00000923L

//
// v27 Pilot callsign
//
// 337 <You got all of them!  Great job!
// v27 337
//
// MessageId: AICF_GROUND_GOOD_US_4
//
// MessageText:
//
//  v27 337%0
//
#define AICF_GROUND_GOOD_US_4            0x00000924L

//
// v27 Pilot callsign
//
// 338 <Good work!  Remaining enemy forces are surrendering!>
// v27 338
//
// MessageId: AICF_GROUND_GOOD_US_5
//
// MessageText:
//
//  v27 338%0
//
#define AICF_GROUND_GOOD_US_5            0x00000925L

//
// v27 Pilot callsign
//
// 340 <be advised, target area appears clear>
// v27 340
//
// MessageId: AICF_FAC_TARGET_CLEAR
//
// MessageText:
//
//  v27 340%0
//
#define AICF_FAC_TARGET_CLEAR            0x00000926L

//
// v27 Pilot callsign
//
// 341 <copy RTB>
// v27 341
//
// MessageId: AICF_COPY_RTB
//
// MessageText:
//
//  v27 341%0
//
#define AICF_COPY_RTB                    0x00000927L

//
// 326 <Hit>
// 326
//
// MessageId: AICF_HIT
//
// MessageText:
//
//  326%0
//
#define AICF_HIT                         0x00000928L

//
// 327 <Target Destroyed>
// 327
//
// MessageId: AICF_TARGET_DESTROYED
//
// MessageText:
//
//  327%0
//
#define AICF_TARGET_DESTROYED            0x00000929L

//
// 328 <Target Damaged>
// 328
//
// MessageId: AICF_TARGET_DAMAGED
//
// MessageText:
//
//  328%0
//
#define AICF_TARGET_DAMAGED              0x0000092AL

//
// v1 Pilot callsign
//
// 322 <releasing smoke mark>
// v1 322
//
// MessageId: AICF_FAC_RELEASE_SMOKE
//
// MessageText:
//
//  v1 322%0
//
#define AICF_FAC_RELEASE_SMOKE           0x0000092BL

//
// v1 Pilot callsign
//
// 323 <laser on, sparkle>
// v1 323
//
// MessageId: AICF_FAC_LASER_ON
//
// MessageText:
//
//  v1 323%0
//
#define AICF_FAC_LASER_ON                0x0000092CL

//
// v1 Pilot callsign
//
// 324 <target illuminated>
// v1 324
//
// MessageId: AICF_FAC_ILLUMINATED
//
// MessageText:
//
//  v1 324%0
//
#define AICF_FAC_ILLUMINATED             0x0000092DL

//
// v1 Pilot callsign
//
// 325 <smoke placed>
// v1 325
//
// MessageId: AICF_FAC_SMOKE_PLACED
//
// MessageText:
//
//  v1 325%0
//
#define AICF_FAC_SMOKE_PLACED            0x0000092EL

//
// v27 Pilot callsign
//
// 307 <nine line as follow, IP>
//
// r1 Waypoint
//
// 308 <target bearing>
//
// v7 bearing
//
// 309 <distance>
//
// v10 range
//
// 310 <Target elevation>
//
// r3 elevation
//
// r4 target type
//
// 311 <sending coordinates on dolly>
// v27 307 r1 308 v7 309 v10 310 r3 r4 311
//
// MessageId: AICF_START_9_LINE
//
// MessageText:
//
//  v27 307 r1 308 v7 309 v10 310 r3 r4 311%0
//
#define AICF_START_9_LINE                0x0000092FL

//
// 312 <no marker point>
// 312
//
// MessageId: AICF_NO_MARKER
//
// MessageText:
//
//  312%0
//
#define AICF_NO_MARKER                   0x00000930L

//
// 313 <smoke marker>
//
// v7 bearing
//
// v10 range
// 313 v7 v10
//
// MessageId: AIC_SMOKE_BEARING
//
// MessageText:
//
//  313 v7 v10%0
//
#define AIC_SMOKE_BEARING                0x00000931L

//
// 314 <smoke marker on target>
// 314
//
// MessageId: AICF_MARKER_ON_TARGET
//
// MessageText:
//
//  314%0
//
#define AICF_MARKER_ON_TARGET            0x00000932L

//
// 315 <No friendlies in the area>
// 315
//
// MessageId: AICF_NO_FRIENDS
//
// MessageText:
//
//  315%0
//
#define AICF_NO_FRIENDS                  0x00000933L

//
// 316 <Friendlies bearing>
//
// v7 bearing
//
// v10 range
// 316 v7 v10
//
// MessageId: AICF_FRIENDS_BEARING
//
// MessageText:
//
//  316 v7 v10%0
//
#define AICF_FRIENDS_BEARING             0x00000934L

//
// 317 <no threats in area>
// 317
//
// MessageId: AICF_NO_THREATS
//
// MessageText:
//
//  317%0
//
#define AICF_NO_THREATS                  0x00000935L

//
// 318 <threat>
//
// r8 threat type
//
// v7 bearing
//
// v10 range
// 316 v7 v10
//
// MessageId: AICF_THREATS_BEARING
//
// MessageText:
//
//  316 v7 v10%0
//
#define AICF_THREATS_BEARING             0x00000936L

//
// v27 Pilot callsign
//
// 319 <four lines as follows>
// v27 319
//
// MessageId: AICF_START_4_LINE
//
// MessageText:
//
//  v27 319%0
//
#define AICF_START_4_LINE                0x00000937L

//
// 308 <target bearing>
//
// v7 bearing
//
// 309 <distance>
//
// v10 range
//
// r4 target type
// 308 v7 309 v10 r4
//
// MessageId: AICF_TARGET_4_LINE
//
// MessageText:
//
//  308 v7 309 v10 r4%0
//
#define AICF_TARGET_4_LINE               0x00000938L

//
// v3 wingman number
//
// 711 < Winchester>
// v1 711
//
// MessageId: AICF_WINCHESTER
//
// MessageText:
//
//  v1 711%0
//
#define AICF_WINCHESTER                  0x00000939L

//
// v3 wingman number
//
// 709 <bingo fuel>
// v3 709
//
// MessageId: AICF_BINGO_FUEL2
//
// MessageText:
//
//  v3 709%0
//
#define AICF_BINGO_FUEL2                 0x0000093AL

//
// v3 wingman number
//
// 669 <returning to base>
// v3 669
//
// MessageId: AICF_FLIGHT_RTB
//
// MessageText:
//
//  v3 669%0
//
#define AICF_FLIGHT_RTB                  0x0000093BL

//
// v6 Wingman callsign
//
// 544 <we've been hit!>
// v5 544
//
// MessageId: AICF_WING_HIT2
//
// MessageText:
//
//  v5 544%0
//
#define AICF_WING_HIT2                   0x0000093CL

//
// v6 Wingman callsign
//
// 500 Bandit, Bandit
// v6 500
//
// MessageId: AICF_BANDIT_BANDIT
//
// MessageText:
//
//  v6 500%0
//
#define AICF_BANDIT_BANDIT               0x0000093DL

//
// v6 Position Number intense
//
// 512 <SAM launch>
// v6 512
//
// MessageId: AICF_COMM_SAM_LAUNCH
//
// MessageText:
//
//  v6 512%0
//
#define AICF_COMM_SAM_LAUNCH             0x0000093EL

//
// v6 Position Number intense
//
// 521 <Heads Up, Missile Launch>
// v6 521
//
// MessageId: AICF_COMM_MISSILE_LAUNCH
//
// MessageText:
//
//  v6 521%0
//
#define AICF_COMM_MISSILE_LAUNCH         0x0000093FL

//
// v3 number in group
//
// 510 <Triple A fire spotted>
// v3 510
//
// MessageId: AICF_WING_NUM_AAA_SPOT_4
//
// MessageText:
//
//  v3 510%0
//
#define AICF_WING_NUM_AAA_SPOT_4         0x00000940L

//
// v3 Wingman number
//
// 571 <Spike, Spike>
// v3 571
//
// MessageId: AICF_COMM_SPIKE_SPIKE
//
// MessageText:
//
//  v3 571%0
//
#define AICF_COMM_SPIKE_SPIKE            0x00000941L

//
// v2 Callsign with Number
//
// 507 <ground targets at>
//
// v24 clock position
// v2 507 v24
//
// MessageId: AICF_WING_GRND_TRG_SPOT_1
//
// MessageText:
//
//  v2 507 v24%0
//
#define AICF_WING_GRND_TRG_SPOT_1        0x00000942L

//
// v3 wingman number
//
// 507 <ground targets at>
//
// v7 bearing
// v3 507 v7
//
// MessageId: AICF_WING_GRND_TRG_SPOT_1A
//
// MessageText:
//
//  v3 507 v7%0
//
#define AICF_WING_GRND_TRG_SPOT_1A       0x00000943L

//
// v2 Callsign with Number
//
// 504 <surface ships at>
//
// v24 clock position
// v2 504 v24
//
// MessageId: AICF_WING_SHIP_SPOT_1
//
// MessageText:
//
//  v2 504 v24%0
//
#define AICF_WING_SHIP_SPOT_1            0x00000944L

//
// v3 wingman number
//
// 504 <surface ships at>
//
// v7 bearing
// v3 504 v7
//
// MessageId: AICF_WING_SHIP_SPOT_1A
//
// MessageText:
//
//  v3 504 v7%0
//
#define AICF_WING_SHIP_SPOT_1A           0x00000945L

//
// v2 Callsign with Number
//
// 593 <strobe>
//
// v24 clock position
// v2 504 v24
//
// MessageId: AICF_WING_STROBE_DIR
//
// MessageText:
//
//  v2 593 v24%0
//
#define AICF_WING_STROBE_DIR             0x00000946L

//
// v3 Wingman Number
//
// 593 <strobe>
//
// v7 bearing
// v3 593 v7
//
// MessageId: AICF_WING_STROBE_DIRA
//
// MessageText:
//
//  v3 593 v7%0
//
#define AICF_WING_STROBE_DIRA            0x00000947L

//
// 329 <Miss, short>
//
// r1 distance in feet (place digits) <feet>
// 329 r1
//
// MessageId: AICF_MISS_SHORT
//
// MessageText:
//
//  329 r1%0
//
#define AICF_MISS_SHORT                  0x00000948L

//
// 330 <Miss, long>
//
// r1 distance in feet (place digits) <feet>
// 330 r1
//
// MessageId: AICF_MISS_LONG
//
// MessageText:
//
//  330 r1%0
//
#define AICF_MISS_LONG                   0x00000949L

//
// 331 <Miss, left>
//
// r1 distance in feet (place digits) <feet>
// 331 r1
//
// MessageId: AICF_MISS_LEFT
//
// MessageText:
//
//  331 r1%0
//
#define AICF_MISS_LEFT                   0x0000094AL

//
// 332 <Miss, right>
//
// r1 distance in feet (place digits) <feet>
// 332 r1
//
// MessageId: AICF_MISS_RIGHT
//
// MessageText:
//
//  332 r1%0
//
#define AICF_MISS_RIGHT                  0x0000094BL

//
// v28 Callsign with Number off targetnum
//
// 304 <climb and maintain one two zero zero feet, when established turn to downwind>
//
// v7 bearing
// v28 304 v7
//
// MessageId: AICF_FINAL_BOLTER1
//
// MessageText:
//
//  v28 304 v7%0
//
#define AICF_FINAL_BOLTER1               0x0000094CL

//
// v28 Callsign with Number off targetnum
//
// 305 <left to bearing>
//
// v7 bearing
// v28 305 v7
//
// MessageId: AICF_FINAL_BOLTER2
//
// MessageText:
//
//  v28 305 v7%0
//
#define AICF_FINAL_BOLTER2               0x0000094DL

//
// v28 Callsign with Number
//
// 319 <new final bearing is>
//
// v7 bearing
// v28 319 v7
//
// MessageId: AICF_NEW_FINAL_BEARING
//
// MessageText:
//
//  v28 319 v7%0
//
#define AICF_NEW_FINAL_BEARING           0x0000094EL

//
// v2 Callsign with Number
//
// 689 <Laser on>
// v2 689
//
// MessageId: AICF_WING_LASER_ON
//
// MessageText:
//
//  v2 689%0
//
#define AICF_WING_LASER_ON               0x0000094FL

//
// v2 Callsign with Number
//
// 690 <Laser off>
// v2 690
//
// MessageId: AICF_WING_LASER_OFF
//
// MessageText:
//
//  v2 690%0
//
#define AICF_WING_LASER_OFF              0x00000950L

//
// 641 <delcare>
// 641
//
// MessageId: AICF_DECLARE2
//
// MessageText:
//
//  641%0
//
#define AICF_DECLARE2                    0x00000951L

//
// v28 Callsign off targetnum with number
//
// 380 <Hostile>
// v28 380
//
// MessageId: AICF_HOSTILE
//
// MessageText:
//
//  v28 380%0
//
#define AICF_HOSTILE                     0x00000952L

//
// v28 Callsign off targetnum with number
//
// 382 <Bogey>
// v28 382
//
// MessageId: AICF_BOGEY
//
// MessageText:
//
//  v28 382%0
//
#define AICF_BOGEY                       0x00000953L

//
// v28 Callsign off targetnum with number
//
// 384 <Unable, clean>
// v28 384
//
// MessageId: AICF_UNABLE_CLEAN
//
// MessageText:
//
//  v28 384%0
//
#define AICF_UNABLE_CLEAN                0x00000954L

//
// v28 Callsign off targetnum with number
//
// 381 <Bandit>
// v28 381
//
// MessageId: AICF_BANDIT
//
// MessageText:
//
//  v28 381%0
//
#define AICF_BANDIT                      0x00000955L

//
// v28 Callsign off targetnum with number
//
// 383 <Friendly>
// v28 383
//
// MessageId: AICF_FRIENDLY
//
// MessageText:
//
//  v28 383%0
//
#define AICF_FRIENDLY                    0x00000956L

//
// v2 Callsign with Number
//
// 584 <on station>
// v2 584
//
// MessageId: AICF_ON_STATION
//
// MessageText:
//
//  v2 584%0
//
#define AICF_ON_STATION                  0x00000957L

//
// v2 Callsign with Number
//
// 653 <request relief>
// v2 653
//
// MessageId: AICF_REQUEST_RELIEF
//
// MessageText:
//
//  v2 653%0
//
#define AICF_REQUEST_RELIEF              0x00000958L

//
// v28 Callsign with Number off targetnum
//
// 259 <Strike, your signal is divert.  Acknowledge.>
// Not Translated
//
// MessageId: AICF_STRIKE_DIVERT
//
// MessageText:
//
//  v28 259%0
//
#define AICF_STRIKE_DIVERT               0x00000959L

//
// v28 Callsign with Number off targetnum
//
// 413 <Understand you are declaring an emergency>
// 413 v28
//
// MessageId: AICF_ARRIVAL_EMERGENCY
//
// MessageText:
//
//  413 v28%0
//
#define AICF_ARRIVAL_EMERGENCY           0x0000095AL

//
// v28 Callsign with Number off targetnum
//
// 292 <sweet and sweet.  Continue outbound
// v28 292
//
// MessageId: AICF_STRIKE_SWEET_OUT
//
// MessageText:
//
//  v28 292%0
//
#define AICF_STRIKE_SWEET_OUT            0x0000095BL

//
// v2 Callsign with Number
//
// 293 <switching control, contract>
//
// v28 Callsign with Number off targetnum
// v2 293 v28
//
// MessageId: AICF_STRIKE_SWITCH_CONTACT
//
// MessageText:
//
//  v2 293 v28%0
//
#define AICF_STRIKE_SWITCH_CONTACT       0x0000095CL

//
// v2 Callsign with Number
//
// 727 <Airborne>
// v2 727
//
// MessageId: AICF_AIRBORNE
//
// MessageText:
//
//  v2 727%0
//
#define AICF_AIRBORNE                    0x0000095DL

//
// v2 Callsign with Number
//
// 728 <passing two point five>
// v2 728
//
// MessageId: AICF_PASSING_2_5
//
// MessageText:
//
//  v2 728%0
//
#define AICF_PASSING_2_5                 0x0000095EL

//
// v2 Callsign with Number
//
// 729 <Arcing>
// v2 729
//
// MessageId: AICF_ARCING
//
// MessageText:
//
//  v2 729%0
//
#define AICF_ARCING                      0x0000095FL

//
// v2 Callsign with Number
//
// 730 <OutBound>
// v2 730
//
// MessageId: AICF_OUTBOUND
//
// MessageText:
//
//  v2 730%0
//
#define AICF_OUTBOUND                    0x00000960L

//
// v2 Callsign with Number
//
// 750 <up for checks>
// v2 750
//
// MessageId: AICF_UP_FOR_CHECKS
//
// MessageText:
//
//  v2 750%0
//
#define AICF_UP_FOR_CHECKS               0x00000961L

//
// v3 Number in group
//
// 576 <ducks away!>
// v3 576
//
// MessageId: AICF_WING_DUCKS_AWAY
//
// MessageText:
//
//  v3 576%0
//
#define AICF_WING_DUCKS_AWAY             0x00000962L

//
// v2 Callsign with Number
//
// 531 <dug in guns spotted, rolling in hot>
// v2 531
//
// MessageId: AICF_WING_ATTACKING_DUG_GUNS
//
// MessageText:
//
//  v2 531%0
//
#define AICF_WING_ATTACKING_DUG_GUNS     0x00000963L

//
// v2 Callsign with Number
//
// 534 <dug in SAM site in sight, commencing attack.>
// v2 534
//
// MessageId: AICF_WING_ATTACKING_DUG_SAM
//
// MessageText:
//
//  v2 534%0
//
#define AICF_WING_ATTACKING_DUG_SAM      0x00000964L

//
// v2 Callsign with Number
//
// 533 <triple A spotted, attacking>
// v2 533
//
// MessageId: AICF_WING_ATTACKING_AAA
//
// MessageText:
//
//  v2 533%0
//
#define AICF_WING_ATTACKING_AAA          0x00000965L

//
// v2 Callsign with Number
//
// 530 <Enemy armor in sight, in hot
// v2 530
//
// MessageId: AICF_WING_ATTACKING_ARMOR
//
// MessageText:
//
//  v2 530%0
//
#define AICF_WING_ATTACKING_ARMOR        0x00000966L

//
// v2 Callsign with Number
//
// 532 <trucks in sight, commencing attack>
// v2 532
//
// MessageId: AICF_WING_ATTACKING_TRUCKS
//
// MessageText:
//
//  v2 532%0
//
#define AICF_WING_ATTACKING_TRUCKS       0x00000967L

//
// v2 Callsign with Number
//
// 535 <mobile SAM site spotted, rolling in hot>
// v2 535
//
// MessageId: AICF_WING_ATTACKING_MOBILE_SAM
//
// MessageText:
//
//  v2 535%0
//
#define AICF_WING_ATTACKING_MOBILE_SAM   0x00000968L

//
// v2 Callsign with Number
//
// 536 <enemy ship in sight, attacking
// v2 536
//
// MessageId: AICF_WING_ATTACKING_SHIP
//
// MessageText:
//
//  v2 536%0
//
#define AICF_WING_ATTACKING_SHIP         0x00000969L

//
// v2 Callsign with Number
//
// 537 <enemy tin can spotted, attacking.
// v2 537
//
// MessageId: AICF_WING_ATTACKING_CAN
//
// MessageText:
//
//  v2 537%0
//
#define AICF_WING_ATTACKING_CAN          0x0000096AL

//
// 320 <Vampires inbound bearing>
//
// v7 bearing
// 320 v7
//
// MessageId: AICF_STRIKE_VAMPIRE
//
// MessageText:
//
//  320 v7%0
//
#define AICF_STRIKE_VAMPIRE              0x0000096BL

//
// 321 <Additional Vampires now inbound bearing>
//
// v7 bearing
// 321 v7
//
// MessageId: AICF_STRIKE_ADDITIONAL_VAMPIRES
//
// MessageText:
//
//  321 v7%0
//
#define AICF_STRIKE_ADDITIONAL_VAMPIRES  0x0000096CL

//
// 250 <Inbound hostiles, Birds Affirm>
// 250
//
// MessageId: AICF_STIKE_INBOUND_HOSTILES
//
// MessageText:
//
//  250%0
//
#define AICF_STIKE_INBOUND_HOSTILES      0x0000096DL

//
// 251 <Inbound vampires, Birds Affirm>
// 251
//
// MessageId: AICF_STRIKE_INBOUND_VAMPIRES
//
// MessageText:
//
//  251%0
//
#define AICF_STRIKE_INBOUND_VAMPIRES     0x0000096EL

//
// 252 <Birds away>
// 252
//
// MessageId: AICF_STRIKE_BIRDS_AWAY
//
// MessageText:
//
//  252%0
//
#define AICF_STRIKE_BIRDS_AWAY           0x0000096FL

//
// 253 <Hit Alpha!>
// 253
//
// MessageId: AICF_STRIKE_HIT_ALPHA
//
// MessageText:
//
//  253%0
//
#define AICF_STRIKE_HIT_ALPHA            0x00000970L

//
// 254 <Hit Alpha!  Vampire impact on mother!>
// 254
//
// MessageId: AICF_STRIKE_HIT_ALPHA_VAMPS
//
// MessageText:
//
//  254%0
//
#define AICF_STRIKE_HIT_ALPHA_VAMPS      0x00000971L

//
// 255 <All chicks, Mother is being abandonded at this time>
// 255
//
// MessageId: AICF_STRIKE_MOTHER_ABANDONED
//
// MessageText:
//
//  255%0
//
#define AICF_STRIKE_MOTHER_ABANDONED     0x00000972L

//
// 256 <Mother sick, flight deck closed>
// Not Translated
//
// MessageId: AICF_STRIKE_MOTHER_SICK
//
// MessageText:
//
//  256%0
//
#define AICF_STRIKE_MOTHER_SICK          0x00000973L

//
// 257 <Green deck, flight deck now open>
// 257
//
// MessageId: AICF_STRIKE_GREEN_DECK
//
// MessageText:
//
//  257%0
//
#define AICF_STRIKE_GREEN_DECK           0x00000974L

//
// 258 <All chicks, Strike, Your signal is divert, mother is closed.>
// 258
//
// MessageId: AICF_STRIKE_MOTHER_CLOSED
//
// MessageText:
//
//  258%0
//
#define AICF_STRIKE_MOTHER_CLOSED        0x00000975L

//
// SIMULATION-MULTIPLAYER INFO
//
// MessageId: NET_NEW_HOST
//
// MessageText:
//
//  You have become the host%0
//
#define NET_NEW_HOST                     0x00000976L

//
// MessageId: NET_LOST_HOST
//
// MessageText:
//
//  Connection to the session is lost%0
//
#define NET_LOST_HOST                    0x00000977L

//
// MessageId: NET_JOIN_GAME
//
// MessageText:
//
//  %1 has joined%0
//
#define NET_JOIN_GAME                    0x00000978L

//
// MessageId: NET_QUIT_GAME
//
// MessageText:
//
//  %1 has quit%0
//
#define NET_QUIT_GAME                    0x00000979L

//
// MessageId: NET_IN_FLIGHT
//
// MessageText:
//
//  %1 is in flight%0
//
#define NET_IN_FLIGHT                    0x0000097AL

//
// MessageId: NET_CRASHED
//
// MessageText:
//
//  %1 just crashed%0
//
#define NET_CRASHED                      0x0000097BL

//
// MessageId: NET_HIT_YOU
//
// MessageText:
//
//  You just hit %1%0
//
#define NET_HIT_YOU                      0x0000097CL

//
// MessageId: NET_HIT_ME
//
// MessageText:
//
//  You were just hit by %1%0
//
#define NET_HIT_ME                       0x0000097DL

//
// MessageId: NET_KILL_YOU
//
// MessageText:
//
//  You just %1 %2%0
//
#define NET_KILL_YOU                     0x0000097EL

//
// MessageId: NET_KILL_ME
//
// MessageText:
//
//  You were just %1 by %2%0
//
#define NET_KILL_ME                      0x0000097FL

//
// MessageId: NET_KILL_BY
//
// MessageText:
//
//  %3 was just %1 by %2%0
//
#define NET_KILL_BY                      0x00000980L

//
// MessageId: NET_KILL_VERB
//
// MessageText:
//
//  killed%0
//
#define NET_KILL_VERB                    0x00000981L

//
// MessageId: NET_KILL_VERB1
//
// MessageText:
//
//  fried%0
//
#define NET_KILL_VERB1                   0x00000982L

//
// MessageId: NET_KILL_VERB2
//
// MessageText:
//
//  nailed%0
//
#define NET_KILL_VERB2                   0x00000983L

//
// MessageId: NET_KILL_VERB3
//
// MessageText:
//
//  wasted%0
//
#define NET_KILL_VERB3                   0x00000984L

//
// MessageId: NET_KILL_VERB4
//
// MessageText:
//
//  splashed%0
//
#define NET_KILL_VERB4                   0x00000985L

//
// MessageId: NET_KILL_VERB5
//
// MessageText:
//
//  barbecued%0
//
#define NET_KILL_VERB5                   0x00000986L

//
// MessageId: NET_KILL_VERB6
//
// MessageText:
//
//  splattered%0
//
#define NET_KILL_VERB6                   0x00000987L

//
// MessageId: NET_KILL_VERB7
//
// MessageText:
//
//  incinerated%0
//
#define NET_KILL_VERB7                   0x00000988L

