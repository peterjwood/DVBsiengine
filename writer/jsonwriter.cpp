// writer.cpp: implementation of the writer class.
//
//////////////////////////////////////////////////////////////////////

#include "jsonwriter.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static const char * cSIJSONStringTable[IDS_MAX_STRING]=
{
/*    IDS_UNKNOWN             */   "Unknown",
/*    IDS_DATA                */   "Data",
/*    IDS_BATMAIN             */   "Bouquet Association Table",
/*    IDS_BATLOOP             */   "Bouquet Descriptor Loop",
/*    IDS_LENGTH              */   "Length",
/*    IDS_TSLOOPLEN           */   "Transport Stream Loop Length",
/*    IDS_TRANSSTREAM         */   "Transport Streams",
/*    IDS_ID                  */   "ID",
/*    IDS_ONID                */   "Original Network ID",
/*    IDS_CAT                 */   "CA Table",
/*    IDS_ECM                 */   "ECM Table",
/*    IDS_DIREQUEST           */   "Download Info Request Message",
/*    IDS_BUFSIZ              */   "Buffer size",
/*    IDS_MAXBLOCK            */   "Max block size",
/*    IDS_PDLEN               */   "Private data length",
/*    IDS_DII                 */   "Download Info Response or DII Message",
/*    IDS_DLID                */   "Download ID",
/*    IDS_BLOCKSIZE           */   "Block size",
/*    IDS_WINDOWSIZE          */   "Window size",
/*    IDS_ACKPER              */   "Ack Period",
/*    IDS_DLWIN               */   "Download Window",
/*    IDS_DLSC                */   "Download Scenario",
/*    IDS_NUMMOD              */   "Number of modules",
/*    IDS_MODID               */   "Module ID",
/*    IDS_SIZE                */   "Size",
/*    IDS_VER                 */   "Version",
/*    IDS_MTO                 */   "Module Time Out",
/*    IDS_BTO                 */   "Block Time Out",
/*    IDS_MBT                 */   "Min Block Time",
/*    IDS_TAPSC               */   "Taps count",
/*    IDS_USE                 */   "Use",
/*    IDS_ASSTAG              */   "Assoc TAG",
/*    IDS_SELLEN              */   "Selector length",
/*    IDS_USEINFLEN           */   "User info length",
/*    IDS_USEINFO             */   "User info",
/*    IDS_COMPDESC            */   "Compression descriptor",
/*    IDS_COMPMETH            */   "Compression method",
/*    IDS_ORIGSIZ             */   "Original Size",
/*    IDS_OTHERDESC           */   "Other descriptor",
/*    IDS_DESCLEN             */   "Descriptor length",
/*    IDS_MODINFLEN           */   "Module info length",
/*    IDS_MODINFO             */   "Module info",
/*    IDS_DDB                 */   "Download data block",
/*    IDS_BLOCKNUM            */   "Block Number",
/*    IDS_BIOPMESS            */   "BIOP message",
/*    IDS_COMPBIOPMESS        */   "Compressed BIOP message",
/*    IDS_BIOPMAGFAIL         */   "BIOP Message magic number fail",
/*    IDS_DDREQMESS           */   "Download Data Request Message",
/*    IDS_REASON              */   "Reason",
/*    IDS_DSI                 */   "Download Server Initiate Message",
/*    IDS_SERVID              */   "Server ID",
/*    IDS_DESCCOUNT           */   "Descriptor count",
/*    IDS_SELTYPE             */   "Selector type",
/*    IDS_DESCTYPE            */   "Descriptor Type",
/*    IDS_SPECTYPE            */   "Specifier type",
/*    IDS_SPECDATA            */   "Specifier data",
/*    IDS_MODEL               */   "Model",
/*    IDS_SUBDESCCOUNT        */   "SubDescriptor count",
/*    IDS_SUBDESCTYPE         */   "SubDescriptor Type",
/*    IDS_IOPIOR              */   "IOP::IOR:",
/*    IDS_PROF                */   "Profile",
/*    IDS_TAG                 */   "ID TAG",
/*    IDS_DATALEN             */   "Data length",
/*    IDS_BYTEORD             */   "Byte order",
/*    IDS_COMPCOUNT           */   "Component count",
/*    IDS_UNKCOMP             */   "Unknown Component",
/*    IDS_BIOPOL              */   "BIOP:ObjectLocation 0x49534f50",
/*    IDS_CARID               */   "Carousel ID",
/*    IDS_MAJVER              */   "Major version",
/*    IDS_MINVER              */   "Minor Version",
/*    IDS_OBJKEYLEN           */   "Object key length",
/*    IDS_OBJKEY              */   "Object key",
/*    IDS_DSMCONN             */   "DSM:ConnBinder 0x49534f40",
/*    IDS_TAP                 */   "DSM:Tap",
/*    IDS_TRANSID             */   "Transaction ID",
/*    IDS_TIMEOUT             */   "Timeout",
/*    IDS_SERVGATE            */   "Service Gateway",
/*    IDS_TYPE                */   "Type",
/*    IDS_DLCANCEL            */   "Download Cancel Message",
/*    IDS_COMPATDESC          */   "Compatibility descriptor",
/*    IDS_MESSIZE             */   "Message size",
/*    IDS_OBJKINDLEN          */   "Object kind length",
/*    IDS_OBJKIND             */   "Object kind",
/*    IDS_DSMCCTAB            */   "DSMCC Table",
/*    IDS_PROTDESC            */   "Protocol discriminator",
/*    IDS_MESSID              */   "Message ID",
/*    IDS_SECT                */   "Section",
/*    IDS_ADAPTLEN            */   "Adaption len",
/*    IDS_MESSLEN             */   "Message len",
/*    IDS_ADAPT               */   "Adaption",
/*    IDS_FILE                */   "file",
/*    IDS_OBJINFLEN           */   "Object info length",
/*    IDS_OBJINFO             */   "Object info",
/*    IDS_FILECONSIZ          */   "File Content size",
/*    IDS_MESSBODLEN          */   "Message body length",
/*    IDS_CONLEN              */   "Content length",
/*    IDS_CONTENT             */   "Content",
/*    IDS_DIR                 */   "dir",
/*    IDS_BINDCOUNT           */   "Bindings count",
/*    IDS_BINDTYPE            */   "Binding Type",
/*    IDS_STR                 */   "str",
/*    IDS_STE                 */   "ste",
/*    IDS_SRG                 */   "srg",
/*    IDS_IDLEN               */   "ID Len",
/*    IDS_BIOPNAME            */   "BIOP Name",
/*    IDS_SERVCONT            */   "Service Context",
/*    IDS_SERVCONTLICOUNT     */   "Service Context List Count",
/*    IDS_CONTID              */   "Context ID",
/*    IDS_EIT                 */   "EIT",
/*    IDS_ACTUAL              */   "actual",
/*    IDS_OTHER               */   "other",
/*    IDS_ACTUALSCH           */   "actual schedule",
/*    IDS_OTHERSCH            */   "other schedule",
/*    IDS_CUR                 */   "Current",
/*    IDS_NEXT                */   "Next",
/*    IDS_SERVICEID           */   "Service ID",
/*    IDS_TSID                */   "Transport Stream  ID",
/*    IDS_NID                 */   "Network ID",
/*    IDS_SLASTSEC            */   "Segment Last Section",
/*    IDS_LASTTABID           */   "Last Table ID",
/*    IDS_EVENTID             */   "Event ID",
/*    IDS_STIME               */   "Start Time",
/*    IDS_DUR                 */   "Duration",
/*    IDS_UNDEF               */   "Running status - undefined",
/*    IDS_NOTRUN              */   "Running status - not running",
/*    IDS_SOON                */   "Running status - starts in a few seconds",
/*    IDS_PAUSING             */   "Running status - pausing",
/*    IDS_RUN                 */   "Running status - running",
/*    IDS_RESERR              */   "Running status - reserved -- ERROR",
/*    IDS_CAACT               */   "CA Active",
/*    IDS_CANACT              */   "CA Not Active",
/*    IDS_EVDESCLOOP          */   "Event Descriptor Loop - ",
/*    IDS_NIT                 */   "NIT",
/*    IDS_NDL                 */   "Network Descriptor Loop -",
/*    IDS_PAT                 */   "PAT",
/*    IDS_PROGNUM             */   "Program Number",
/*    IDS_PMTPID              */   "Program map PID",
/*    IDS_NETPID              */   "Network PID",
/*    IDS_PMT                 */   "PMT",
/*    IDS_PCRPID              */   "PCR PID",
/*    IDS_PROGINF             */   "Program Info",
/*    IDS_PROGSTR             */   "Program Streams",
/*    IDS_STRTYPE             */   "Stream Type",
/*    IDS_RESERV              */   "Reserved",
/*    IDS_VIDTYPE             */   "Video Type 1",
/*    IDS_VIDTYPE1            */   "Video Type 2",
/*    IDS_AUDIO               */   "Audio 3",
/*    IDS_AUDIO1              */   "Audio 4",
/*    IDS_13818PRIV           */   "13818 private sections",
/*    IDS_PDINPES             */   "Private data in PES",
/*    IDS_MHEG                */   "MHEG",
/*    IDS_ANNADSMCC           */   "Annex A DSMCC",
/*    IDS_ITUT                */   "ITU-T 222",
/*    IDS_138186A             */   "13818-6 A",
/*    IDS_138186B             */   "13818-6 B",
/*    IDS_138186C             */   "13818-6 C",
/*    IDS_138186D             */   "13818-6 D",
/*    IDS_138181AUX           */   "13818-1 Aux",
/*    IDS_STRRESERV           */   "reserved",
/*    IDS_STRUSRPRIV          */   "user private",
/*    IDS_ELEMPID             */   "Elementary PID",
/*    IDS_ESINF               */   "ES Info",
/*    IDS_SDT                 */   "SDT",
/*    IDS_SERVICES            */   "Services",
/*    IDS_EITSCHDATA          */   "EIT Schedule data",
/*    IDS_EITPRESFOLL         */   "EIT present following data",
/*    IDS_UNTABTYPE           */   "Unknown Table type",
/*    IDS_TIDEXT              */   "Table ID Extension",
/*    IDS_LASTSEC             */   "Last Section",
/*    IDS_CURSEC              */   "Current Section",
/*    IDS_HEX                 */   "Data in HEX form",
/*    IDS_CHAR                */   "Data in character form",
/*    IDS_PID                 */   "PID",
/*    IDS_VSDESC              */   "Video Stream Descriptor",
/*    IDS_MULTIFRATE          */   "Multiple Frame Rate",
/*    IDS_TRUE                */   "True",
/*    IDS_FALSE               */   "False",
/*    IDS_FRCODE              */   "Frame rate code",
/*    IDS_MP1ONLY             */   "MPEG 1 only",
/*    IDS_CONPARAMF           */   "Constrained parameter flag",
/*    IDS_STILLPIC            */   "Still picture",
/*    IDS_PROFANDLEVEL        */   "Profile and level",
/*    IDS_CRFORMAT            */   "Chroma format",
/*    IDS_FREXT               */   "Frame rate extension",
/*    IDS_ASDESC              */   "Audio Stream Descriptor",
/*    IDS_FREEFORM            */   "Free format",
/*    IDS_IDFLAG              */   "ID flag",
/*    IDS_LAYER               */   "Layer",
/*    IDS_VARRATE             */   "Variable rate",
/*    IDS_HIEDESC             */   "Hierachy Descriptor",
/*    IDS_LAYIND              */   "Layer index",
/*    IDS_EMBLAYIND           */   "Embedded layer index",
/*    IDS_VERTSIZ             */   "Vertical size",
/*    IDS_HEICHAN             */   "Hierachy channel",
/*    IDS_REGDESC             */   "Registration Descriptor",
/*    IDS_FORMATID            */   "Format identifier",
/*    IDS_ADDITINFO           */   "Additional info",
/*    IDS_DSALIGNMENTDESC     */   "Data Stream Alignment Descriptor",
/*    IDS_TBGD                */   "Target background grid descriptor",
/*    IDS_HORSIZ              */   "Horizontal size",
/*    IDS_ASPECT              */   "Aspect ratio",
/*    IDS_VIDWINDESC          */   "Video Window Descriptor",
/*    IDS_HORIZOFF            */   "Horizontal offset",
/*    IDS_VERTOFF             */   "Vertical offset",
/*    IDS_PRIO                */   "Priority",
/*    IDS_CADESC              */   "CA Descriptor",
/*    IDS_CASYSID             */   "CA System ID",
/*    IDS_CAPID               */   "CA PID",
/*    IDS_CADATA              */   "CA DATA",
/*    IDS_ISOLANGDESC         */   "ISO language descriptor -",
/*    IDS_ISOCODE             */   "ISO Code",
/*    IDS_AUDTYPE             */   "Audio Type",
/*    IDS_CLEANEFF            */   "Clean Effects",
/*    IDS_HEARIMP             */   "Hearing Impaired",
/*    IDS_VISIMP              */   "Visual Impaired commentary",
/*    IDS_SYSCLKDESC          */   "System Clock Descriptor -",
/*    IDS_EXTCLKREF           */   "External clock reference - ",
/*    IDS_CLKACCINT           */   "Clock accuracy integer",
/*    IDS_CLKACCEXP           */   "Clock accuracy exponent",
/*    IDS_MXBUFFUTILDESC      */   "Multiplex Buffer Utilisation Descriptor",
/*    IDS_BNDVALID            */   "Bound valid flag",
/*    IDS_LTWOFFLOW           */   "LTW off lower",
/*    IDS_LTWOFFUPP           */   "LTW off upper",
/*    IDS_COPYDESC            */   "Copyright Descriptor",
/*    IDS_COPYID              */   "Copyright ID",
/*    IDS_PRIVDAT             */   "Private data",
/*    IDS_MAXBRDESC           */   "Maximum Bitrate Descriptor",
/*    IDS_BR                  */   "bitrate",
/*    IDS_PDIDESC             */   "Private Data Indicator Descriptor",
/*    IDS_SMOOTHBUFDESC       */   "Smoothing Buffer Descriptor",
/*    IDS_STDDESC             */   "STD Descriptor",
/*    IDS_LEAKVAL             */   "Leak Valid",
/*    IDS_IBPDESC             */   "IBP Descriptor",
/*    IDS_NETNAME             */   "Network Name",
/*    IDS_SLDESC              */   "Service List Descriptor",
/*    IDS_SERVICETYPE         */   "Service Type",
/*    IDS_STUFFDESC           */   "Stuffing descriptor",
/*    IDS_SATDSDESC           */   "Satellite delivery system descriptor",
/*    IDS_CABDSDESC           */   "Cable delivery system descriptor",
/*    IDS_SERVDESC            */   "Service descriptor",
/*    IDS_SERVICETYPE2        */   "Service Type",
/*    IDS_USERDEF             */   "User defined",
/*    IDS_DIGTV               */   "Digital TV",
/*    IDS_DIGRAD              */   "Digital Radio sound service",
/*    IDS_TELETEXT            */   "Teletext",
/*    IDS_NVODREF             */   "NVOD Reference",
/*    IDS_NVODTS              */   "NVOD time shifted",
/*    IDS_MOSAIC              */   "mosaic",
/*    IDS_PALSIG              */   "PAL coded signal",
/*    IDS_SECAMSIG            */   "SECAM coded signal",
/*    IDS_DD2MAC              */   "D-D2 MAC",
/*    IDS_FMRAD               */   "FM Radio",
/*    IDS_NTSCCOD             */   "NTSC Coded",
/*    IDS_DATBROAD            */   "Data Broadcast",
/*    IDS_SERVPROV            */   "Service Provider",
/*    IDS_SERVNAME            */   "Service Name",
/*    IDS_COUNTRYAVAIDESC     */   "Country availability descriptor",
/*    IDS_AVAIL               */   "available",
/*    IDS_NOTAVAIL            */   "not available",
/*    IDS_LINKDESC            */   "Linkage descriptor",
/*    IDS_LINKS               */   "Links",
/*    IDS_TSID2               */   "Transport Stream ID",
/*    IDS_ONID2               */   "Original Network ID",
/*    IDS_SHORTEVDESC         */   "Short event descriptor",
/*    IDS_LINKTYPE            */   "Linkage Type",
/*    IDS_LANGCODE            */   "Language Code",
/*    IDS_EVNAMELEN           */   "Event Name length",
/*    IDS_EVNAME              */   "Event Name",
/*    IDS_EVTEXT              */   "Event Text",
/*    IDS_EVTEXTLEN           */   "Event Text length",
/*    IDS_EXTEVENTDESC        */   "Extended event descriptor",
/*    IDS_DESCNUMLDNUM        */   "Descriptor number Top Nibble  Last descriptor number Bottom Nibble",
/*    IDS_COMPONDESC          */   "Component descriptor",
/*    IDS_STREAMCONT0         */   "Stream content 0 reserved",
/*    IDS_STREAMCONT10        */   "Stream content 1 - type 0 reserved",
/*    IDS_STREAMCONT11        */   "Stream content 1 - type 1 video, 4:3 aspect ratio",
/*    IDS_STREAMCONT12        */   "Stream content 1 - type 2 video, 16:9 aspect ratio with pan vectors",
/*    IDS_STREAMCONT13        */   "Stream content 1 - type 3 video, 16:9 aspect ratio without pan vectors",
/*    IDS_STREAMCONT14        */   "Stream content 1 - type 4 video, > 16:9 aspect ratio",
/*    IDS_STREAMCONT15        */   "Stream content 1 - type > 5 reserved",
/*    IDS_STREAMCONT20        */   "Stream content 2 - type 0 reserved",
/*    IDS_STREAMCONT21        */   "Stream content 2 - type 1 audio, single mono channel",
/*    IDS_STREAMCONT22        */   "Stream content 2 - type 2 audio, dual mono channel",
/*    IDS_STREAMCONT23        */   "Stream content 2 - type 3 audio, stereo (2 channel)",
/*    IDS_STREAMCONT24        */   "Stream content 2 - type 4 audio, multi-lingual, multi-channel",
/*    IDS_STREAMCONT25        */   "Stream content 2 - type 5 audio, surround sound",
/*    IDS_STREAMCONT240       */   "Stream content 2 - type 0x40 audio description for the visually impaired",
/*    IDS_STREAMCONT241       */   "Stream content 2 - type 0x41 audio for the hard of hearing",
/*    IDS_STREAMCONT26        */   "user defined audio",
/*    IDS_STREAMCONT27        */   "Stream content 2 - type reserved",
/*    IDS_STREAMCONT30        */   "Stream content 3 - type 0 reserved",
/*    IDS_STREAMCONT31        */   "Stream content 3 - type 1 EBU Teletext subtitles",
/*    IDS_STREAMCONT32        */   "Stream content 3 - type 2 associated EBU Teletext",
/*    IDS_STREAMCONT310       */   "Stream content 3 - type 0x10 DVB subtitles (normal) with no monitor aspect ratio criticality",
/*    IDS_STREAMCONT311       */   "Stream content 3 - type 0x11 DVB subtitles (normal) for display on 4:3 aspect ratio monitor",
/*    IDS_STREAMCONT312       */   "Stream content 3 - type 0x12 DVB subtitles (normal) for display on 16:9 aspect ratio monitor",
/*    IDS_STREAMCONT313       */   "Stream content 3 - type 0x13 DVB subtitles (normal) for display on 2.21:1 aspect ratio monitor",
/*    IDS_STREAMCONT320       */   "Stream content 3 - type 0x20 DVB subtitles (for the hard of hearing) with no monitor aspect ratio criticality",
/*    IDS_STREAMCONT321       */   "Stream content 3 - type 0x21 DVB subtitles (for the hard of hearing) for display on 4:3 aspect ratio monitor",
/*    IDS_STREAMCONT322       */   "Stream content 3 - type 0x22 DVB subtitles (for the hard of hearing) for display on 16:9 aspect ratio monitor",
/*    IDS_STREAMCONT323       */   "Stream content 3 - type 0x23 DVB subtitles (for the hard of hearing) for display on 2.21:1 aspect ratio monitor",
/*    IDS_STREAMCONT324       */   "Stream content 3 - type subtitle reserved",
/*    IDS_STREAMCONT4         */   "Stream content > 3 reserved",
/*    IDS_STREAMIDDESC        */   "Stream Identifier descriptor",
/*    IDS_CAIDDESC            */   "CA ID descriptor",
/*    IDS_CONTENTDESC         */   "Content descriptor",
/*    IDS_CONTENT1            */   "Movie/Drama:",
/*    IDS_CONTENT2            */   "News/Current affairs:",
/*    IDS_CONTENT3            */   "Show/Game show:",
/*    IDS_CONTENT4            */   "Sports:",
/*    IDS_CONTENT5            */   "Children's/Youth programmes:",
/*    IDS_CONTENT6            */   "Music/Ballet/Dance:",
/*    IDS_CONTENT7            */   "Arts/Culture (without music):",
/*    IDS_CONTENT8            */   "Social/Political issues/Economics:",
/*    IDS_CONTENT9            */   "Children's/Youth programmes: Education/ Science/Factual topics:",
/*    IDS_CONTENT10           */   "Leisure hobbies:",
/*    IDS_CONTENT11           */   "Special Characteristics:",
/*    IDS_CONTENT12           */   "User defined",
/*    IDS_PARRATDESC          */   "Parental Rating descriptor -",
/*    IDS_COUNTRYCODE         */   "Country Code",
/*    IDS_RATING              */   "Rating",
/*    IDS_SUBDESC             */   "Subtitiling Descriptor -",
/*    IDS_SUBTYPE             */   "Subtitile Type - ",
/*    IDS_EBUTTXSUB           */   "EBU Teletext subtitles",
/*    IDS_ASSEBUTTX           */   "Associated EBU Teletext",
/*    IDS_DVBSUB1             */   "DVB subtitles (normal) with no monitor aspect ratio criticality",
/*    IDS_DVBSUB2             */   "DVB subtitles (normal) for display on 4:3 aspect ratio monitor",
/*    IDS_DVBSUB3             */   "DVB subtitles (normal) for display on 16:9 aspect ratio monitor",
/*    IDS_DVBSUB4             */   "DVB subtitles (normal) for display on 2.21:1 aspect ratio monitor",
/*    IDS_DVBSUB5             */   "DVB subtitles (for the hard of hearing) with no monitor aspect ratio criticality",
/*    IDS_DVBSUB6             */   "DVB subtitles (for the hard of hearing) for display on 4:3 aspect ratio monitor",
/*    IDS_DVBSUB7             */   "DVB subtitles (for the hard of hearing) for display on 16:9 aspect ratio monitor",
/*    IDS_DVBSUB8             */   "DVB subtitles (for the hard of hearing) for display on 2.21:1 aspect ratio monitor",
/*    IDS_RESERVVAL           */   "Reserved",
/*    IDS_CENTFREQ            */   "Center frequency",
/*    IDS_COMPPAGEID          */   "Composition page ID",
/*    IDS_ANCPAGEID           */   "Ancillary page ID",
/*    IDS_TERRDELIVSYSDESC    */   "Terrestrial delivery system descriptor -",
/*    IDS_7MBAND              */   "7Mhz bandwidth",
/*    IDS_8MBAND              */   "8Mhz bandwidth",
/*    IDS_RESBAND             */   "reserved bandwidth",
/*    IDS_INVALID             */   "Invalid",
/*    IDS_NONHIER             */   "Non-hierarchical",
/*    IDS_a1                  */   "a=1",
/*    IDS_a2                  */   "a=2",
/*    IDS_a3                  */   "a=3",
/*    IDS_HPCR                */   "HP Code Rate - ",
/*    IDS_LPCR                */   "LP Code Rate - ",
/*    IDS_GUARDINT            */   "Guard Interval - ",
/*    IDS_TRANS8K             */   "Transmission mode 8k",
/*    IDS_TRANS2K             */   "Transmission mode 2k",
/*    IDS_OTHERFREQINUSE      */   "Other freq in use",
/*    IDS_NOOTHERFREQINUSE    */   "No other freq in use",
/*    IDS_MULTCOMPDESC        */   "Multilingual component descriptor -",
/*    IDS_COMPTAG             */   "Component tag",
/*    IDS_DESCRIPTION         */   "Description - ",
/*    IDS_PRIVDATSPECDESC     */   "Private data specifier descriptor -",
/*    IDS_FREQLISTDESC        */   "Frequency list descriptor -",
/*    IDS_FREQLIST            */   "Frequencies",
/*    IDS_CODETYPE            */   "Coding type - ",
/*    IDS_SAT                 */   "Satellite",
/*    IDS_CAB                 */   "Cable",
/*    IDS_TERR                */   "Terrestrial",
/*    IDS_DATABROADDESC       */   "Data broadcast Descriptor -",
/*    IDS_ID2                 */   "ID",
/*    IDS_SELECTOR            */   "Selector -",
/*    IDS_DATABROADIDDESC     */   "Data broadcast ID Descriptor",
/*    IDS_UKNDESC             */   "Unknown Descriptor Type",
/*    IDS_SATFREQ             */   "Frequency",
/*    IDS_ORBPOS              */   "Orbital position",
/*    IDS_EAST                */   "East",
/*    IDS_WEST                */   "West",
/*    IDS_POLAR               */   "Polarisation",
/*    IDS_QPSK                */   "QPSK",
/*    IDS_16QAM               */   "16 QAM",
/*    IDS_64QAM               */   "64 QAM",
/*    IDS_MOD                 */   "Modulation",
/*    IDS_SYMBRATE            */   "Symbol rate",
/*    IDS_FECINNER            */   "FEC inner",
/*    IDS_FECOUTER            */   "FEC Outer",
/*    IDS_CIRC                */   "Circular",
/*    IDS_LINEAR              */   "Linear",
/*    IDS_RIGHT               */   "Right",
/*    IDS_LEFT                */   "Left",
/*    IDS_PES                 */   "PES data - PID",
/*    IDS_PESID               */   "PES Stream ID",
/*    IDS_PESEXT              */   "PES Extension present",
/*    IDS_PESSCRAM            */   "PES Scrambling",
/*    IDS_PESPRIO             */   "PES Priority",
/*    IDS_PESDATAALIGN        */   "Data aligned",
/*    IDS_PESCOPY             */   "Copyright set",
/*    IDS_PESORIG             */   "Original",
/*    IDS_PTSDTS              */   "PTS DTS Flags",
/*    IDS_PESESCR             */   "ESCR set",
/*    IDS_ESRATE              */   "ES Rate",
/*    IDS_DSMTRICK            */   "DSM Trick Mode",
/*    IDS_ADDCOPY             */   "Additional copy info",
/*    IDS_PESCRCFLAG          */   "PES CRC set",
/*    IDS_PESEXTFLAG          */   "PES Extension flag set",
/*    IDS_PESHDRLEN           */   "PES Header Length",
/*    IDS_PESPTSTOP           */   "PTS Top",
/*    IDS_PESPTSVAL           */   "PTS Value",
/*    IDS_PESDTSTOP           */   "DTS Top",
/*    IDS_PESDTSVAL           */   "PES DTS Value",
/*    IDS_PESESCRVAL          */   "PES ESCR value",
/*    IDS_PESESRATE           */   "PES ES Rate",
/*    IDS_PESADDCOPYINF       */   "PES Additional copy info",
/*    IDS_PREVPESCRC          */   "Previous PES CRC",
/*    IDS_PESEXTFLAGVAL       */   "PES Extension Flag value",
/*    IDS_PESPACKSEQCOUNTVAL  */   "Packet Sequence counter data",
/*    IDS_PESSTDBUFFVAL       */   "STD Buffer data",
/*    IDS_PESEXTDATA2         */   "PES Ext 2 data",
/*    IDS_PRIVATEPES1         */   "Private PES Stream 1",
/*    IDS_PESDATA             */   "PES Data",
/*    IDS_PESPAD              */   "Padding stream",
/*    IDS_PRIVATEPES2         */   "Private PES stream 2",
/*    IDS_PESAUDIO            */   "MPEG 1 or 2 Audio",
/*    IDS_PESVIDEO            */   "MPEG 1 or 2 Video",
/*    IDS_FSTUNNELDESC        */   "Freesat Tunnelled Data Descriptor",
/*    IDS_FSEITPF             */   "EITpf",
/*    IDS_FSEITSHED           */   "EITschedule",
/*    IDS_FSEITPP             */   "EITpf++",
/*    IDS_FSSDT               */   "SDT",
/*    IDS_FSBAT               */   "BAT",
/*    IDS_FSTDT               */   "TDT",
/*    IDS_FSTOT               */   "TOT",
/*    IDS_FSNIT               */   "NIT",
/*    IDS_FSRES               */   "Reserved",
/*    IDS_FSRECAUD            */   "Recordable Audio",
/*    IDS_FSRECVID            */   "Recordable Video",
/*    IDS_FSPOSTCODE          */   "Postcode Tables",
/*    IDS_FSRNT               */   "RNT",
/*    IDS_FSALTTUNNELDESC     */   "Freesat Alt Tunnelled Data Descriptor",
/*    IDS_FSEITPF2            */   "EITpf",
/*    IDS_FSEITSHED2          */   "EITschedule",
/*    IDS_FSEITPP2            */   "EITpf++",
/*    IDS_FSSDT2              */   "SDT",
/*    IDS_FSBAT2              */   "BAT",
/*    IDS_FSTDT2              */   "TDT",
/*    IDS_FSTOT2              */   "TOT",
/*    IDS_FSNIT2              */   "NIT",
/*    IDS_FSRES2              */   "Reserved",
/*    IDS_FSRECAUD2           */   "Recordable Audio",
/*    IDS_FSRECVID2           */   "Recordable Video",
/*    IDS_FSPOSTCODE2         */   "Postcode Tables",
/*    IDS_FSRNT2              */   "RNT",
/*    IDS_FSLINKDESC          */   "Free satellite linkage descriptor",
/*    IDS_FSSTNDLL            */   "Standard loop len",
/*    IDS_FSLINKTYPE          */   "Freesat Linkage type",
/*    IDS_FSTNDLL             */   "Tunnelled loop len",
/*    IDS_FSOUI               */   "OUI",
/*    IDS_FSRNAMEDESC         */   "Region Name descriptor",
/*    IDS_FSREGIONID          */   "Region ID",
/*    IDS_UPDATETYPE          */   "Update Type",
/*    IDS_UDVERSIONINGDATA    */   "Update Versioning data",
/*    IDS_2BYTEDATA           */   "DATA ",
/*    IDS_12                  */   "1/2",
/*    IDS_23                  */   "2/3",
/*    IDS_34                  */   "3/4",
/*    IDS_56                  */   "5/6",
/*    IDS_78                  */   "7/8",
/*    IDS_1_32                */   "1/32",
/*    IDS_1_16                */   "1/16",
/*    IDS_1_8                 */   "1/8",
/*    IDS_1_4                 */   "1/4",
/*    IDS_CAROUSELIDDESC      */   "Carousel ID Descriptor",
/*    IDS_ASSOCIATIONTAGDESC  */   "Association Tag Descriptor",
/*    IDS_DEFASSTAGDESC       */   "Deferred Association Tag Descriptor",
/*    IDS_DESCRIPTORS         */   "Descriptors",
};




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static jsonwriter NextLevel[256];

char writebuffer[64*1024];

bool jsonwriter::write(char* message)
{
	return ProcessData(message);
}
bool jsonwriter::ProcessData(char* message)
{	
	int i;

	for (i = 0; (i < indent); i++)
		fprintf(output,"\t");

	if (firstitem)
	{
		fprintf(output, "{%s",message);
		firstitem=false;
	}
	else if (lastitem)
	{
		fprintf(output, "%s",message);
		lastitem=false;
		firstitem=true;
	}
	else
		fprintf(output, ",%s",message);
	
	fprintf(output,"\n");
	return true;
}
void jsonwriter::startlist(component_ids id)
{
	if (id < IDS_MAX_STRING)
	{
		sprintf(writebuffer,"\"%s\" :[",cSIJSONStringTable[id] );
	}
	else
		sprintf(writebuffer,"Invalid string_id %.2X ",id);
	ProcessData(writebuffer);

	writer::startlist(id);
}
void jsonwriter::listitem()
{
	if (listcount)
	{
		for (int i = 0; (i < indent); i++)
			fprintf(output,"\t");
		fprintf(output,",\n");
	}
	writer::listitem();
}

void jsonwriter::endlist()
{
	for (int i = 0; (i < indent); i++)
		fprintf(output,"\t");

	fprintf(output, "]\n");

	writer::endlist();
}
void jsonwriter::enditem()
{
}
writer *jsonwriter::child()
{
	NextLevel[indent].init();
	NextLevel[indent].indent = indent + 1;
	NextLevel[indent].output = output;
	return &NextLevel[indent];
}
void jsonwriter::removechild(writer *child)
{
	if (child == &NextLevel[indent])
	{
		if (NextLevel[indent].lastitem==false)
		{
			NextLevel[indent].lastitem=true;
		}
		if (NextLevel[indent].firstitem==false)
			child->ProcessData("}");
	}
	else
		printf("Trying to free wrong child\n");
}

bool jsonwriter::write(component_ids id, unsigned int val)
{
	if (id < IDS_MAX_STRING)
	{
		sprintf(writebuffer,"\"%s\" :\"0x%.2X\"",cSIJSONStringTable[id] ,val);
	}
	else
		sprintf(writebuffer,"Invalid string_id %.2X ",id);

	return ProcessData(writebuffer);
}

bool jsonwriter::write(component_ids id)
{
#if 0
	return true;
#else

	if (id < IDS_MAX_STRING)
	{
		sprintf(writebuffer,"\"%s\":\"true\"",cSIJSONStringTable[id]);
	}
	else
		sprintf(writebuffer,"Unknown ID %d",id);

	return ProcessData(writebuffer);
#endif
}

bool jsonwriter::writetime(component_ids id,unsigned char *data, unsigned int len)
{
	if ((id != IDS_STIME) || (len != 5))
		sprintf(writebuffer, "writetime called with wrong ID %d or len %d",id,len);
	else
	{
		unsigned short tmp,year,month,day;

		tmp = (data[0]<<8) + data[1];
		year = (tmp - 15078) / 365;
		month = ( tmp - 14956 - (year * 365) )/ 30;
		day = tmp - 14956 - (year * 365) - (month * 30 );

		if ((month == 14) || (month == 15))
			tmp = 1;
		else
			tmp = 0;

		year += tmp + 1900;
		month = month - 1 - (12 * tmp);
		sprintf(writebuffer,"\"%s\":\"%X:%.2X:%.2X %d/%d/%d\"",cSIJSONStringTable[id],data[2],data[3],data[4],day,month,year);
	}
	return ProcessData(writebuffer);
}
bool jsonwriter::writeduration(component_ids id,unsigned char *data, unsigned int len)
{
	if ((id != IDS_DUR) || (len != 3))
		sprintf(writebuffer, "writeduration called with wrong ID %d or len %d",id,len);
	else
	{
		sprintf(writebuffer,"\"%s\":\"%X:%.2X:%.2X\"",cSIJSONStringTable[id],data[0],data[1],data[2]);
	}
	return ProcessData(writebuffer);
}


bool jsonwriter::bindata(component_ids id,unsigned char *data, unsigned int len)
{
	unsigned int pos;

	if (id == IDS_STIME)
		return writetime(id,data,len);

	if (id == IDS_DUR)
		return writeduration(id,data,len);

	sprintf(writebuffer,"\"%s\":\"",cSIJSONStringTable[id]);

	if (data != NULL)
	{
		for (pos=0;pos < len; pos++)
		{
			sprintf(writebuffer,"%s%.2X", writebuffer,data[pos]);
		}
		strcat(writebuffer,"\"");
	}
	return ProcessData(writebuffer);
}

bool jsonwriter::chardata(component_ids id,char *data, unsigned int len)
{
	unsigned int pos;

	sprintf(writebuffer,"\"%s\":\"",cSIJSONStringTable[id]);

	for (pos=0;pos < len; pos++)
	{
		if (data[pos] > 0x1f) 
		{
			sprintf(writebuffer,"%s%c",writebuffer,data[pos]);
		}
		else
			strcat(writebuffer,".");

	}
	strcat(writebuffer,"\"");
	return ProcessData(writebuffer);
}
