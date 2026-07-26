#ifndef CLASSACTIONCD_H
#define CLASSACTIONCD_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define TXT_HELP 272
#define TXT_ICONIFY 273
#define TXT_PROJECT 274
#define TXT_QUIT 275
#define TXT_PREFERENCES 276
#define TXT_ABOUT 277
#define ERR_NOLIB 279
#define TXT_INFORMATION 280
#define TXT_COPYRIGHT 281
#define TXT_SURE 282
#define TXT_DIR 283
#define TXT_MEMORY 286
#define TXT_UNKNOWN 287
#define TXT_REQERROR 288
#define TXT_YESNO 289
#define TXT_SUREERROR 290
#define TXT_ASSIGN 291
#define TXT_CLONE 292
#define TXT_DRAWER 293
#define TXT_VOLUME 294
#define TXT_OK 295
#define TXT_CANCEL 296
#define TXT_DELOK 297
#define MSG_MKDIR 298
#define TXT_NOCONFIG 299
#define TXT_CONFIGCORRUPT 300
#define TXT_CONFIGNOTVALID 301
#define ERR_GENERAL 303
#define ERR_NOICON 304
#define ERR_WRONGPATH 305
#define ERR_FILENOTFOUND 306
#define ERR_NOAREXX 307
#define ERR_NOXFD 308
#define ERR_NOTCRUNCHED 309
#define ERR_NOTDATA 310
#define ERR_NOTDECR 311
#define TXT_RENAME 313
#define TXT_FILEEXISTS 315
#define TXT_CONFIRMREPLACE 317
#define TXT_COPYICONS 318
#define TXT_CREATEICONS 319
#define TXT_DISPLAYINFO 320
#define TXT_SETTINGS 321
#define TXT_SHOWDRAWERSFIRST 322
#define TXT_BUTTDEV 335
#define TXT_BUTTN 336
#define TXT_BUTTMKDIR 337
#define TXT_BUTTPATH 339
#define TXT_PROG 345
#define TXT_COPYING 346
#define TXT_DELETING 347
#define TXT_MOVING 348
#define TXT_SNAPSHOTWINDOW 350
#define TXT_YESALLNONENO 351
#define TXT_HASICON 353
#define ERR_NOCOPY 356
#define ERR_NODEL 357
#define ERR_NOMOVE 358
#define TXT_ENTPAT 359
#define TXT_NEWFILE 361
#define TXT_OLDFILE 362
#define TXT_MOVEAS 363
#define TXT_DATESTRING 364
#define TXT_DISPLAYTIME 365
#define TXT_BUTTABORT 368
#define TXT_DRAGBAR 369
#define TXT_TOOLBAR 370
#define TXT_VISIBLE 371
#define TXT_BYTESSEL 373
#define TXT_CHECKING 374
#define TXT_DDSELECTED 375
#define TXT_NOTENOUGHSPACE 376
#define TXT_CHECKFIT 385
#define TXT_DELETEICONS 386
#define TXT_GENERAL 387
#define TXT_HIGHDIRS 388
#define TXT_ICONS 389
#define TXT_LISTERS 390
#define TXT_SHOWIMAGES 391
#define TXT_USED 392
#define TXT_BUTTPARENT 393
#define TXT_NAME 396
#define TXT_SIZE 397
#define TXT_DATE 398
#define TXT_BITS 399
#define TXT_BUTTDIR 400
#define TXT_BUTTDIR2 401
#define TXT_COPY 402
#define TXT_CUT 403
#define TXT_EDIT 404
#define TXT_PASTE 405
#define TXT_PATHNOTIFY 406
#define TXT_SEARCH 407
#define TXT_SELALL 408
#define TXT_SELNONE 409
#define TXT_SELTOGGLE 410
#define TXT_SELTYPE 411
#define TXT_SELPATTERN 412
#define TXT_DELETE 413
#define TXT_COPYAS 414
#define TXT_FILE 415
#define TXT_MOVE 416
#define TXT_PACK 417
#define TXT_BUTTREFRESH 418
#define TXT_CREATEARCHIVE 419
#define TXT_CREATESUBDIR 420
#define TXT_DELOKSINGLE 421
#define TXT_GENERIC 422
#define TXT_BUTTACTIVATE 423
#define TXT_BUTTDEACTIVATE 424
#define MSG_COMMENT 427
#define MSG_ARCHIVE 428
#define MSG_EXECUTABLE 430
#define MSG_OPENWINDOW 431
#define MSG_READABLE 432
#define MSG_ROOTDIR 433
#define MSG_SCRIPT 434
#define MSG_VOLUMES 435
#define MSG_WRITABLE 436
#define MSG_DELETABLE 437
#define MSG_COMMENT8 438
#define MSG_DATE8 439
#define MSG_DATENOTVALID 440
#define MSG_FILEALREADYEXISTS 441
#define MSG_NAME8 442
#define MSG_PROTECTION8 443
#define MSG_CANCEL1 444
#define MSG_OK1 445
#define MSG_NOTRASHCANAVAILABLE 446
#define MSG_BUTTFUELGAUGE 448
#define MSG_COUNTINGFILES 449
#define TXT_DELOKTRASH 450
#define TXT_DELOKSINGLETRASH 451
#define MSG_DEPROTECTFILE 452
#define ERR_NOMKDIR 453
#define MSG_VERSION 454
#define MSG_NOTAVAILABLE 455
#define MSG_VERSION8 456
#define MSG_COPYANDDELETE 457
#define MSG_PULLDOWNMENU 458
#define MSG_REPLACEVERSION 459
#define MSG_SAVE1 460
#define MSG_SHOWVERSION 461
#define MSG_COUNTFILES 462
#define MSG_CREATE1 464
#define MSG_ICON 465
#define MSG_NAMEOFDIR8 466
#define MSG_PLEASEENTERDIRNAME 467
#define MSG_ASSIGNTITLE 470
#define MSG_COPYTO 471
#define MSG_ERR_ASSIGN 472
#define MSG_OPENWITH 473
#define MSG_PATHS 474
#define MSG_VISITEDPATHS 475
#define MSG_ASSIGNEXISTS 476
#define MSG_ASSIGNMAKE 477
#define MSG_ASSIGNREPLACEADD 478
#define MSG_ASSIGNREMOVE 479
#define MSG_ASSIGNWANTREMOVE 480
#define MSG_CLOSEWINDOW 481
#define MSG_CONFIRMCHECKSIZE 482
#define MSG_DDIRSDFILES 483
#define MSG_NORENAME 484
#define ERR_WRONGPREFS 485
#define MSG_CONFIRMDELETE 486
#define MSG_TOOLS 487
#define MSG_NOCOPYTOSUBDIR 488

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define TXT_HELP_STR "Help"
#define TXT_ICONIFY_STR "Iconify"
#define TXT_PROJECT_STR "Project"
#define TXT_QUIT_STR "Quit"
#define TXT_PREFERENCES_STR "Preferences"
#define TXT_ABOUT_STR "About"
#define ERR_NOLIB_STR "Library not found.\nCheck the requirements."
#define TXT_INFORMATION_STR "Information"
#define TXT_COPYRIGHT_STR "%s\n\n© 2003 by Martin R. Elsner & Salim Gasmi\n\nYou can contact the author at :\n\nMartin R. Elsner\nMuesener Strasse 46\n57223 Kreuztal\nGermany\n\nEmail: classaction@martin-elsner.de \nWeb: http://www.martin-elsner.de"
#define TXT_SURE_STR "Are you sure ?"
#define TXT_DIR_STR "Directory"
#define TXT_MEMORY_STR "Not Enough Memory \n\nto load the file"
#define TXT_UNKNOWN_STR "Unknown"
#define TXT_REQERROR_STR "Syntax Error in REQ command"
#define TXT_YESNO_STR "Yes|No"
#define TXT_SUREERROR_STR "Syntax error in SURE command"
#define TXT_ASSIGN_STR "Assign"
#define TXT_CLONE_STR "Copy As"
#define TXT_DRAWER_STR "Drawer"
#define TXT_VOLUME_STR "Volume"
#define TXT_OK_STR "Okay"
#define TXT_CANCEL_STR "Cancel"
#define TXT_DELOK_STR "Do you really want to delete the entries %s, ... ?"
#define MSG_MKDIR_STR "Create new directory"
#define TXT_NOCONFIG_STR "Config file not found."
#define TXT_CONFIGCORRUPT_STR "Your config file is corrupted."
#define TXT_CONFIGNOTVALID_STR "Your config file is not a valid\nClassAction config file."
#define ERR_GENERAL_STR "An unspecified error has occured."
#define ERR_NOICON_STR "There is no icon!"
#define ERR_WRONGPATH_STR "This path does not exist!"
#define ERR_FILENOTFOUND_STR "File not found."
#define ERR_NOAREXX_STR "The ARexx port could not be opened."
#define ERR_NOXFD_STR "Xfdmaster.library not found !!\nThe autodecrunching function\nwill be deactivated."
#define ERR_NOTCRUNCHED_STR "Not crunched."
#define ERR_NOTDATA_STR "Not a data file."
#define ERR_NOTDECR_STR "Not decrunched."
#define TXT_RENAME_STR "Rename"
#define TXT_FILEEXISTS_STR "Do you want to replace"
#define TXT_CONFIRMREPLACE_STR "Confirm replace"
#define TXT_COPYICONS_STR "Copy Icons"
#define TXT_CREATEICONS_STR "Create Icons"
#define TXT_DISPLAYINFO_STR "Display .info"
#define TXT_SETTINGS_STR "Settings"
#define TXT_SHOWDRAWERSFIRST_STR "Show drawers first"
#define TXT_BUTTDEV_STR "Show all devices"
#define TXT_BUTTN_STR "Show the given path"
#define TXT_BUTTMKDIR_STR "Create a new directory"
#define TXT_BUTTPATH_STR "Enter a path to list it"
#define TXT_PROG_STR "ClassAction"
#define TXT_COPYING_STR "Copying"
#define TXT_DELETING_STR "Deleting"
#define TXT_MOVING_STR "Moving"
#define TXT_SNAPSHOTWINDOW_STR "Snapshot Window"
#define TXT_YESALLNONENO_STR "Yes|All|None|No"
#define TXT_HASICON_STR "has already an icon!"
#define ERR_NOCOPY_STR "Could not copy %s !"
#define ERR_NODEL_STR "Could not delete %s !"
#define ERR_NOMOVE_STR "Could not move %s !"
#define TXT_ENTPAT_STR "Enter Pattern"
#define TXT_NEWFILE_STR "New file:"
#define TXT_OLDFILE_STR "Old file:"
#define TXT_MOVEAS_STR "Move As"
#define TXT_DATESTRING_STR "Date as text"
#define TXT_DISPLAYTIME_STR "Display time"
#define TXT_BUTTABORT_STR "Abort the current process"
#define TXT_DRAGBAR_STR "Dragbar"
#define TXT_TOOLBAR_STR "Toolbar"
#define TXT_VISIBLE_STR "Visible"
#define TXT_BYTESSEL_STR "%s bytes selected"
#define TXT_CHECKING_STR "Checking"
#define TXT_DDSELECTED_STR "%d selected / %d available"
#define TXT_NOTENOUGHSPACE_STR "Not enough space!"
#define TXT_CHECKFIT_STR "Check space"
#define TXT_DELETEICONS_STR "Delete icons"
#define TXT_GENERAL_STR "General"
#define TXT_HIGHDIRS_STR "Highlight dirs"
#define TXT_ICONS_STR "Icons"
#define TXT_LISTERS_STR "Listers"
#define TXT_SHOWIMAGES_STR "Show images"
#define TXT_USED_STR "used"
#define TXT_BUTTPARENT_STR "Go to the parent directory"
#define TXT_NAME_STR "Name"
#define TXT_SIZE_STR "Size"
#define TXT_DATE_STR "Date"
#define TXT_BITS_STR "Bits"
#define TXT_BUTTDIR_STR "Source"
#define TXT_BUTTDIR2_STR "Destination"
#define TXT_COPY_STR "Copy"
#define TXT_CUT_STR "Cut"
#define TXT_EDIT_STR "Edit"
#define TXT_PASTE_STR "Paste"
#define TXT_PATHNOTIFY_STR "Update on changes"
#define TXT_SEARCH_STR "Search"
#define TXT_SELALL_STR "Select all"
#define TXT_SELNONE_STR "Select nothing"
#define TXT_SELTOGGLE_STR "Toggle selection"
#define TXT_SELTYPE_STR "Select same type"
#define TXT_SELPATTERN_STR "Select pattern"
#define TXT_DELETE_STR "Delete"
#define TXT_COPYAS_STR "Copy as"
#define TXT_FILE_STR "File"
#define TXT_MOVE_STR "Move"
#define TXT_PACK_STR "Pack"
#define TXT_BUTTREFRESH_STR "Refresh this lister"
#define TXT_CREATEARCHIVE_STR "Create archive"
#define TXT_CREATESUBDIR_STR "Create subdirectory"
#define TXT_DELOKSINGLE_STR "Do you really want to delete the entry %s ?"
#define TXT_GENERIC_STR "Generic"
#define TXT_BUTTACTIVATE_STR "Activate lister"
#define TXT_BUTTDEACTIVATE_STR "Deactivate lister"
#define MSG_COMMENT_STR "Comment"
#define MSG_ARCHIVE_STR "Archive"
#define MSG_EXECUTABLE_STR "Executable"
#define MSG_OPENWINDOW_STR "Open window"
#define MSG_READABLE_STR "Readable"
#define MSG_ROOTDIR_STR "Root directory"
#define MSG_SCRIPT_STR "Script"
#define MSG_VOLUMES_STR "Volumes"
#define MSG_WRITABLE_STR "Writable"
#define MSG_DELETABLE_STR "Deletable"
#define MSG_COMMENT8_STR "Comment:"
#define MSG_DATE8_STR "Date:"
#define MSG_DATENOTVALID_STR "The date is not valid !"
#define MSG_FILEALREADYEXISTS_STR "This file already exists !"
#define MSG_NAME8_STR "Name:"
#define MSG_PROTECTION8_STR "Protection:"
#define MSG_CANCEL1_STR "_Cancel"
#define MSG_OK1_STR "_Ok"
#define MSG_NOTRASHCANAVAILABLE_STR "No trashcan available !"
#define MSG_BUTTFUELGAUGE_STR "Click here to adjust the browser size"
#define MSG_COUNTINGFILES_STR "Counting Files ..."
#define TXT_DELOKTRASH_STR "Do you really want to move the entries %s, ... into the trashcan ?"
#define TXT_DELOKSINGLETRASH_STR "Do you really want to move the entry %s into the trashcan ?"
#define MSG_DEPROTECTFILE_STR "The file %s is write protected.\n Shall I remove the protection for this file?"
#define ERR_NOMKDIR_STR "Could not create directory %s !"
#define MSG_VERSION_STR "Version"
#define MSG_NOTAVAILABLE_STR "not available"
#define MSG_VERSION8_STR "Version:"
#define MSG_COPYANDDELETE_STR "Copy and delete"
#define MSG_PULLDOWNMENU_STR "Pulldown menu"
#define MSG_REPLACEVERSION_STR "Replace older versions"
#define MSG_SAVE1_STR "_Save"
#define MSG_SHOWVERSION_STR "Show version"
#define MSG_COUNTFILES_STR "Count Files"
#define MSG_CREATE1_STR "_Create"
#define MSG_ICON_STR "Icon"
#define MSG_NAMEOFDIR8_STR "Name of the directory:"
#define MSG_PLEASEENTERDIRNAME_STR "Please enter the name of the new directory."
#define MSG_ASSIGNTITLE_STR "Please enter assign title:"
#define MSG_COPYTO_STR "Copy to"
#define MSG_ERR_ASSIGN_STR "Assign could not be created. Please check if this assign already exists."
#define MSG_OPENWITH_STR "Open with"
#define MSG_PATHS_STR "Paths"
#define MSG_VISITEDPATHS_STR "Visited paths"
#define MSG_ASSIGNEXISTS_STR "This assign already exists."
#define MSG_ASSIGNMAKE_STR "Make assign"
#define MSG_ASSIGNREPLACEADD_STR "Replace|Add|Cancel"
#define MSG_ASSIGNREMOVE_STR "Remove assign"
#define MSG_ASSIGNWANTREMOVE_STR "Do you really want to remove this assign?"
#define MSG_CLOSEWINDOW_STR "Please close all windows on this screen."
#define MSG_CONFIRMCHECKSIZE_STR "The volume reports 0 bytes of free space. Do you want to continue?"
#define MSG_DDIRSDFILES_STR "%d/%d directories, %d/%d files"
#define MSG_NORENAME_STR "Could not rename %s !"
#define ERR_WRONGPREFS_STR "This is an old preference file.\nPlease start ClassActionPrefs and save your preferences.\nCheck if you are using the correct version of ClassAction and ClassActionPrefs."
#define MSG_CONFIRMDELETE_STR "Confirm delete"
#define MSG_TOOLS_STR "Tools"
#define MSG_NOCOPYTOSUBDIR_STR "It's not allowed to copy directories into any subdir!"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {TXT_HELP,(STRPTR)TXT_HELP_STR},
    {TXT_ICONIFY,(STRPTR)TXT_ICONIFY_STR},
    {TXT_PROJECT,(STRPTR)TXT_PROJECT_STR},
    {TXT_QUIT,(STRPTR)TXT_QUIT_STR},
    {TXT_PREFERENCES,(STRPTR)TXT_PREFERENCES_STR},
    {TXT_ABOUT,(STRPTR)TXT_ABOUT_STR},
    {ERR_NOLIB,(STRPTR)ERR_NOLIB_STR},
    {TXT_INFORMATION,(STRPTR)TXT_INFORMATION_STR},
    {TXT_COPYRIGHT,(STRPTR)TXT_COPYRIGHT_STR},
    {TXT_SURE,(STRPTR)TXT_SURE_STR},
    {TXT_DIR,(STRPTR)TXT_DIR_STR},
    {TXT_MEMORY,(STRPTR)TXT_MEMORY_STR},
    {TXT_UNKNOWN,(STRPTR)TXT_UNKNOWN_STR},
    {TXT_REQERROR,(STRPTR)TXT_REQERROR_STR},
    {TXT_YESNO,(STRPTR)TXT_YESNO_STR},
    {TXT_SUREERROR,(STRPTR)TXT_SUREERROR_STR},
    {TXT_ASSIGN,(STRPTR)TXT_ASSIGN_STR},
    {TXT_CLONE,(STRPTR)TXT_CLONE_STR},
    {TXT_DRAWER,(STRPTR)TXT_DRAWER_STR},
    {TXT_VOLUME,(STRPTR)TXT_VOLUME_STR},
    {TXT_OK,(STRPTR)TXT_OK_STR},
    {TXT_CANCEL,(STRPTR)TXT_CANCEL_STR},
    {TXT_DELOK,(STRPTR)TXT_DELOK_STR},
    {MSG_MKDIR,(STRPTR)MSG_MKDIR_STR},
    {TXT_NOCONFIG,(STRPTR)TXT_NOCONFIG_STR},
    {TXT_CONFIGCORRUPT,(STRPTR)TXT_CONFIGCORRUPT_STR},
    {TXT_CONFIGNOTVALID,(STRPTR)TXT_CONFIGNOTVALID_STR},
    {ERR_GENERAL,(STRPTR)ERR_GENERAL_STR},
    {ERR_NOICON,(STRPTR)ERR_NOICON_STR},
    {ERR_WRONGPATH,(STRPTR)ERR_WRONGPATH_STR},
    {ERR_FILENOTFOUND,(STRPTR)ERR_FILENOTFOUND_STR},
    {ERR_NOAREXX,(STRPTR)ERR_NOAREXX_STR},
    {ERR_NOXFD,(STRPTR)ERR_NOXFD_STR},
    {ERR_NOTCRUNCHED,(STRPTR)ERR_NOTCRUNCHED_STR},
    {ERR_NOTDATA,(STRPTR)ERR_NOTDATA_STR},
    {ERR_NOTDECR,(STRPTR)ERR_NOTDECR_STR},
    {TXT_RENAME,(STRPTR)TXT_RENAME_STR},
    {TXT_FILEEXISTS,(STRPTR)TXT_FILEEXISTS_STR},
    {TXT_CONFIRMREPLACE,(STRPTR)TXT_CONFIRMREPLACE_STR},
    {TXT_COPYICONS,(STRPTR)TXT_COPYICONS_STR},
    {TXT_CREATEICONS,(STRPTR)TXT_CREATEICONS_STR},
    {TXT_DISPLAYINFO,(STRPTR)TXT_DISPLAYINFO_STR},
    {TXT_SETTINGS,(STRPTR)TXT_SETTINGS_STR},
    {TXT_SHOWDRAWERSFIRST,(STRPTR)TXT_SHOWDRAWERSFIRST_STR},
    {TXT_BUTTDEV,(STRPTR)TXT_BUTTDEV_STR},
    {TXT_BUTTN,(STRPTR)TXT_BUTTN_STR},
    {TXT_BUTTMKDIR,(STRPTR)TXT_BUTTMKDIR_STR},
    {TXT_BUTTPATH,(STRPTR)TXT_BUTTPATH_STR},
    {TXT_PROG,(STRPTR)TXT_PROG_STR},
    {TXT_COPYING,(STRPTR)TXT_COPYING_STR},
    {TXT_DELETING,(STRPTR)TXT_DELETING_STR},
    {TXT_MOVING,(STRPTR)TXT_MOVING_STR},
    {TXT_SNAPSHOTWINDOW,(STRPTR)TXT_SNAPSHOTWINDOW_STR},
    {TXT_YESALLNONENO,(STRPTR)TXT_YESALLNONENO_STR},
    {TXT_HASICON,(STRPTR)TXT_HASICON_STR},
    {ERR_NOCOPY,(STRPTR)ERR_NOCOPY_STR},
    {ERR_NODEL,(STRPTR)ERR_NODEL_STR},
    {ERR_NOMOVE,(STRPTR)ERR_NOMOVE_STR},
    {TXT_ENTPAT,(STRPTR)TXT_ENTPAT_STR},
    {TXT_NEWFILE,(STRPTR)TXT_NEWFILE_STR},
    {TXT_OLDFILE,(STRPTR)TXT_OLDFILE_STR},
    {TXT_MOVEAS,(STRPTR)TXT_MOVEAS_STR},
    {TXT_DATESTRING,(STRPTR)TXT_DATESTRING_STR},
    {TXT_DISPLAYTIME,(STRPTR)TXT_DISPLAYTIME_STR},
    {TXT_BUTTABORT,(STRPTR)TXT_BUTTABORT_STR},
    {TXT_DRAGBAR,(STRPTR)TXT_DRAGBAR_STR},
    {TXT_TOOLBAR,(STRPTR)TXT_TOOLBAR_STR},
    {TXT_VISIBLE,(STRPTR)TXT_VISIBLE_STR},
    {TXT_BYTESSEL,(STRPTR)TXT_BYTESSEL_STR},
    {TXT_CHECKING,(STRPTR)TXT_CHECKING_STR},
    {TXT_DDSELECTED,(STRPTR)TXT_DDSELECTED_STR},
    {TXT_NOTENOUGHSPACE,(STRPTR)TXT_NOTENOUGHSPACE_STR},
    {TXT_CHECKFIT,(STRPTR)TXT_CHECKFIT_STR},
    {TXT_DELETEICONS,(STRPTR)TXT_DELETEICONS_STR},
    {TXT_GENERAL,(STRPTR)TXT_GENERAL_STR},
    {TXT_HIGHDIRS,(STRPTR)TXT_HIGHDIRS_STR},
    {TXT_ICONS,(STRPTR)TXT_ICONS_STR},
    {TXT_LISTERS,(STRPTR)TXT_LISTERS_STR},
    {TXT_SHOWIMAGES,(STRPTR)TXT_SHOWIMAGES_STR},
    {TXT_USED,(STRPTR)TXT_USED_STR},
    {TXT_BUTTPARENT,(STRPTR)TXT_BUTTPARENT_STR},
    {TXT_NAME,(STRPTR)TXT_NAME_STR},
    {TXT_SIZE,(STRPTR)TXT_SIZE_STR},
    {TXT_DATE,(STRPTR)TXT_DATE_STR},
    {TXT_BITS,(STRPTR)TXT_BITS_STR},
    {TXT_BUTTDIR,(STRPTR)TXT_BUTTDIR_STR},
    {TXT_BUTTDIR2,(STRPTR)TXT_BUTTDIR2_STR},
    {TXT_COPY,(STRPTR)TXT_COPY_STR},
    {TXT_CUT,(STRPTR)TXT_CUT_STR},
    {TXT_EDIT,(STRPTR)TXT_EDIT_STR},
    {TXT_PASTE,(STRPTR)TXT_PASTE_STR},
    {TXT_PATHNOTIFY,(STRPTR)TXT_PATHNOTIFY_STR},
    {TXT_SEARCH,(STRPTR)TXT_SEARCH_STR},
    {TXT_SELALL,(STRPTR)TXT_SELALL_STR},
    {TXT_SELNONE,(STRPTR)TXT_SELNONE_STR},
    {TXT_SELTOGGLE,(STRPTR)TXT_SELTOGGLE_STR},
    {TXT_SELTYPE,(STRPTR)TXT_SELTYPE_STR},
    {TXT_SELPATTERN,(STRPTR)TXT_SELPATTERN_STR},
    {TXT_DELETE,(STRPTR)TXT_DELETE_STR},
    {TXT_COPYAS,(STRPTR)TXT_COPYAS_STR},
    {TXT_FILE,(STRPTR)TXT_FILE_STR},
    {TXT_MOVE,(STRPTR)TXT_MOVE_STR},
    {TXT_PACK,(STRPTR)TXT_PACK_STR},
    {TXT_BUTTREFRESH,(STRPTR)TXT_BUTTREFRESH_STR},
    {TXT_CREATEARCHIVE,(STRPTR)TXT_CREATEARCHIVE_STR},
    {TXT_CREATESUBDIR,(STRPTR)TXT_CREATESUBDIR_STR},
    {TXT_DELOKSINGLE,(STRPTR)TXT_DELOKSINGLE_STR},
    {TXT_GENERIC,(STRPTR)TXT_GENERIC_STR},
    {TXT_BUTTACTIVATE,(STRPTR)TXT_BUTTACTIVATE_STR},
    {TXT_BUTTDEACTIVATE,(STRPTR)TXT_BUTTDEACTIVATE_STR},
    {MSG_COMMENT,(STRPTR)MSG_COMMENT_STR},
    {MSG_ARCHIVE,(STRPTR)MSG_ARCHIVE_STR},
    {MSG_EXECUTABLE,(STRPTR)MSG_EXECUTABLE_STR},
    {MSG_OPENWINDOW,(STRPTR)MSG_OPENWINDOW_STR},
    {MSG_READABLE,(STRPTR)MSG_READABLE_STR},
    {MSG_ROOTDIR,(STRPTR)MSG_ROOTDIR_STR},
    {MSG_SCRIPT,(STRPTR)MSG_SCRIPT_STR},
    {MSG_VOLUMES,(STRPTR)MSG_VOLUMES_STR},
    {MSG_WRITABLE,(STRPTR)MSG_WRITABLE_STR},
    {MSG_DELETABLE,(STRPTR)MSG_DELETABLE_STR},
    {MSG_COMMENT8,(STRPTR)MSG_COMMENT8_STR},
    {MSG_DATE8,(STRPTR)MSG_DATE8_STR},
    {MSG_DATENOTVALID,(STRPTR)MSG_DATENOTVALID_STR},
    {MSG_FILEALREADYEXISTS,(STRPTR)MSG_FILEALREADYEXISTS_STR},
    {MSG_NAME8,(STRPTR)MSG_NAME8_STR},
    {MSG_PROTECTION8,(STRPTR)MSG_PROTECTION8_STR},
    {MSG_CANCEL1,(STRPTR)MSG_CANCEL1_STR},
    {MSG_OK1,(STRPTR)MSG_OK1_STR},
    {MSG_NOTRASHCANAVAILABLE,(STRPTR)MSG_NOTRASHCANAVAILABLE_STR},
    {MSG_BUTTFUELGAUGE,(STRPTR)MSG_BUTTFUELGAUGE_STR},
    {MSG_COUNTINGFILES,(STRPTR)MSG_COUNTINGFILES_STR},
    {TXT_DELOKTRASH,(STRPTR)TXT_DELOKTRASH_STR},
    {TXT_DELOKSINGLETRASH,(STRPTR)TXT_DELOKSINGLETRASH_STR},
    {MSG_DEPROTECTFILE,(STRPTR)MSG_DEPROTECTFILE_STR},
    {ERR_NOMKDIR,(STRPTR)ERR_NOMKDIR_STR},
    {MSG_VERSION,(STRPTR)MSG_VERSION_STR},
    {MSG_NOTAVAILABLE,(STRPTR)MSG_NOTAVAILABLE_STR},
    {MSG_VERSION8,(STRPTR)MSG_VERSION8_STR},
    {MSG_COPYANDDELETE,(STRPTR)MSG_COPYANDDELETE_STR},
    {MSG_PULLDOWNMENU,(STRPTR)MSG_PULLDOWNMENU_STR},
    {MSG_REPLACEVERSION,(STRPTR)MSG_REPLACEVERSION_STR},
    {MSG_SAVE1,(STRPTR)MSG_SAVE1_STR},
    {MSG_SHOWVERSION,(STRPTR)MSG_SHOWVERSION_STR},
    {MSG_COUNTFILES,(STRPTR)MSG_COUNTFILES_STR},
    {MSG_CREATE1,(STRPTR)MSG_CREATE1_STR},
    {MSG_ICON,(STRPTR)MSG_ICON_STR},
    {MSG_NAMEOFDIR8,(STRPTR)MSG_NAMEOFDIR8_STR},
    {MSG_PLEASEENTERDIRNAME,(STRPTR)MSG_PLEASEENTERDIRNAME_STR},
    {MSG_ASSIGNTITLE,(STRPTR)MSG_ASSIGNTITLE_STR},
    {MSG_COPYTO,(STRPTR)MSG_COPYTO_STR},
    {MSG_ERR_ASSIGN,(STRPTR)MSG_ERR_ASSIGN_STR},
    {MSG_OPENWITH,(STRPTR)MSG_OPENWITH_STR},
    {MSG_PATHS,(STRPTR)MSG_PATHS_STR},
    {MSG_VISITEDPATHS,(STRPTR)MSG_VISITEDPATHS_STR},
    {MSG_ASSIGNEXISTS,(STRPTR)MSG_ASSIGNEXISTS_STR},
    {MSG_ASSIGNMAKE,(STRPTR)MSG_ASSIGNMAKE_STR},
    {MSG_ASSIGNREPLACEADD,(STRPTR)MSG_ASSIGNREPLACEADD_STR},
    {MSG_ASSIGNREMOVE,(STRPTR)MSG_ASSIGNREMOVE_STR},
    {MSG_ASSIGNWANTREMOVE,(STRPTR)MSG_ASSIGNWANTREMOVE_STR},
    {MSG_CLOSEWINDOW,(STRPTR)MSG_CLOSEWINDOW_STR},
    {MSG_CONFIRMCHECKSIZE,(STRPTR)MSG_CONFIRMCHECKSIZE_STR},
    {MSG_DDIRSDFILES,(STRPTR)MSG_DDIRSDFILES_STR},
    {MSG_NORENAME,(STRPTR)MSG_NORENAME_STR},
    {ERR_WRONGPREFS,(STRPTR)ERR_WRONGPREFS_STR},
    {MSG_CONFIRMDELETE,(STRPTR)MSG_CONFIRMDELETE_STR},
    {MSG_TOOLS,(STRPTR)MSG_TOOLS_STR},
    {MSG_NOCOPYTOSUBDIR,(STRPTR)MSG_NOCOPYTOSUBDIR_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x01\x10\x00\x06"
    TXT_HELP_STR "\x00\x00"
    "\x00\x00\x01\x11\x00\x08"
    TXT_ICONIFY_STR "\x00"
    "\x00\x00\x01\x12\x00\x08"
    TXT_PROJECT_STR "\x00"
    "\x00\x00\x01\x13\x00\x06"
    TXT_QUIT_STR "\x00\x00"
    "\x00\x00\x01\x14\x00\x0C"
    TXT_PREFERENCES_STR "\x00"
    "\x00\x00\x01\x15\x00\x06"
    TXT_ABOUT_STR "\x00"
    "\x00\x00\x01\x17\x00\x2C"
    ERR_NOLIB_STR "\x00\x00"
    "\x00\x00\x01\x18\x00\x0C"
    TXT_INFORMATION_STR "\x00"
    "\x00\x00\x01\x19\x00\xD2"
    TXT_COPYRIGHT_STR "\x00"
    "\x00\x00\x01\x1A\x00\x10"
    TXT_SURE_STR "\x00\x00"
    "\x00\x00\x01\x1B\x00\x0A"
    TXT_DIR_STR "\x00"
    "\x00\x00\x01\x1E\x00\x26"
    TXT_MEMORY_STR "\x00\x00"
    "\x00\x00\x01\x1F\x00\x08"
    TXT_UNKNOWN_STR "\x00"
    "\x00\x00\x01\x20\x00\x1C"
    TXT_REQERROR_STR "\x00"
    "\x00\x00\x01\x21\x00\x08"
    TXT_YESNO_STR "\x00\x00"
    "\x00\x00\x01\x22\x00\x1E"
    TXT_SUREERROR_STR "\x00\x00"
    "\x00\x00\x01\x23\x00\x08"
    TXT_ASSIGN_STR "\x00\x00"
    "\x00\x00\x01\x24\x00\x08"
    TXT_CLONE_STR "\x00"
    "\x00\x00\x01\x25\x00\x08"
    TXT_DRAWER_STR "\x00\x00"
    "\x00\x00\x01\x26\x00\x08"
    TXT_VOLUME_STR "\x00\x00"
    "\x00\x00\x01\x27\x00\x06"
    TXT_OK_STR "\x00\x00"
    "\x00\x00\x01\x28\x00\x08"
    TXT_CANCEL_STR "\x00\x00"
    "\x00\x00\x01\x29\x00\x34"
    TXT_DELOK_STR "\x00\x00"
    "\x00\x00\x01\x2A\x00\x16"
    MSG_MKDIR_STR "\x00\x00"
    "\x00\x00\x01\x2B\x00\x18"
    TXT_NOCONFIG_STR "\x00\x00"
    "\x00\x00\x01\x2C\x00\x20"
    TXT_CONFIGCORRUPT_STR "\x00\x00"
    "\x00\x00\x01\x2D\x00\x3A"
    TXT_CONFIGNOTVALID_STR "\x00\x00"
    "\x00\x00\x01\x2F\x00\x22"
    ERR_GENERAL_STR "\x00"
    "\x00\x00\x01\x30\x00\x12"
    ERR_NOICON_STR "\x00"
    "\x00\x00\x01\x31\x00\x1A"
    ERR_WRONGPATH_STR "\x00"
    "\x00\x00\x01\x32\x00\x10"
    ERR_FILENOTFOUND_STR "\x00"
    "\x00\x00\x01\x33\x00\x24"
    ERR_NOAREXX_STR "\x00"
    "\x00\x00\x01\x34\x00\x52"
    ERR_NOXFD_STR "\x00\x00"
    "\x00\x00\x01\x35\x00\x0E"
    ERR_NOTCRUNCHED_STR "\x00"
    "\x00\x00\x01\x36\x00\x12"
    ERR_NOTDATA_STR "\x00\x00"
    "\x00\x00\x01\x37\x00\x10"
    ERR_NOTDECR_STR "\x00"
    "\x00\x00\x01\x39\x00\x08"
    TXT_RENAME_STR "\x00\x00"
    "\x00\x00\x01\x3B\x00\x18"
    TXT_FILEEXISTS_STR "\x00\x00"
    "\x00\x00\x01\x3D\x00\x10"
    TXT_CONFIRMREPLACE_STR "\x00"
    "\x00\x00\x01\x3E\x00\x0C"
    TXT_COPYICONS_STR "\x00\x00"
    "\x00\x00\x01\x3F\x00\x0E"
    TXT_CREATEICONS_STR "\x00\x00"
    "\x00\x00\x01\x40\x00\x0E"
    TXT_DISPLAYINFO_STR "\x00"
    "\x00\x00\x01\x41\x00\x0A"
    TXT_SETTINGS_STR "\x00\x00"
    "\x00\x00\x01\x42\x00\x14"
    TXT_SHOWDRAWERSFIRST_STR "\x00\x00"
    "\x00\x00\x01\x4F\x00\x12"
    TXT_BUTTDEV_STR "\x00\x00"
    "\x00\x00\x01\x50\x00\x14"
    TXT_BUTTN_STR "\x00"
    "\x00\x00\x01\x51\x00\x18"
    TXT_BUTTMKDIR_STR "\x00\x00"
    "\x00\x00\x01\x53\x00\x18"
    TXT_BUTTPATH_STR "\x00"
    "\x00\x00\x01\x59\x00\x0C"
    TXT_PROG_STR "\x00"
    "\x00\x00\x01\x5A\x00\x08"
    TXT_COPYING_STR "\x00"
    "\x00\x00\x01\x5B\x00\x0A"
    TXT_DELETING_STR "\x00\x00"
    "\x00\x00\x01\x5C\x00\x08"
    TXT_MOVING_STR "\x00\x00"
    "\x00\x00\x01\x5E\x00\x10"
    TXT_SNAPSHOTWINDOW_STR "\x00"
    "\x00\x00\x01\x5F\x00\x10"
    TXT_YESALLNONENO_STR "\x00"
    "\x00\x00\x01\x61\x00\x16"
    TXT_HASICON_STR "\x00\x00"
    "\x00\x00\x01\x64\x00\x14"
    ERR_NOCOPY_STR "\x00"
    "\x00\x00\x01\x65\x00\x16"
    ERR_NODEL_STR "\x00"
    "\x00\x00\x01\x66\x00\x14"
    ERR_NOMOVE_STR "\x00"
    "\x00\x00\x01\x67\x00\x0E"
    TXT_ENTPAT_STR "\x00"
    "\x00\x00\x01\x69\x00\x0A"
    TXT_NEWFILE_STR "\x00"
    "\x00\x00\x01\x6A\x00\x0A"
    TXT_OLDFILE_STR "\x00"
    "\x00\x00\x01\x6B\x00\x08"
    TXT_MOVEAS_STR "\x00"
    "\x00\x00\x01\x6C\x00\x0E"
    TXT_DATESTRING_STR "\x00\x00"
    "\x00\x00\x01\x6D\x00\x0E"
    TXT_DISPLAYTIME_STR "\x00\x00"
    "\x00\x00\x01\x70\x00\x1A"
    TXT_BUTTABORT_STR "\x00"
    "\x00\x00\x01\x71\x00\x08"
    TXT_DRAGBAR_STR "\x00"
    "\x00\x00\x01\x72\x00\x08"
    TXT_TOOLBAR_STR "\x00"
    "\x00\x00\x01\x73\x00\x08"
    TXT_VISIBLE_STR "\x00"
    "\x00\x00\x01\x75\x00\x12"
    TXT_BYTESSEL_STR "\x00"
    "\x00\x00\x01\x76\x00\x0A"
    TXT_CHECKING_STR "\x00\x00"
    "\x00\x00\x01\x77\x00\x1C"
    TXT_DDSELECTED_STR "\x00\x00"
    "\x00\x00\x01\x78\x00\x12"
    TXT_NOTENOUGHSPACE_STR "\x00"
    "\x00\x00\x01\x81\x00\x0C"
    TXT_CHECKFIT_STR "\x00"
    "\x00\x00\x01\x82\x00\x0E"
    TXT_DELETEICONS_STR "\x00\x00"
    "\x00\x00\x01\x83\x00\x08"
    TXT_GENERAL_STR "\x00"
    "\x00\x00\x01\x84\x00\x10"
    TXT_HIGHDIRS_STR "\x00\x00"
    "\x00\x00\x01\x85\x00\x06"
    TXT_ICONS_STR "\x00"
    "\x00\x00\x01\x86\x00\x08"
    TXT_LISTERS_STR "\x00"
    "\x00\x00\x01\x87\x00\x0C"
    TXT_SHOWIMAGES_STR "\x00"
    "\x00\x00\x01\x88\x00\x06"
    TXT_USED_STR "\x00\x00"
    "\x00\x00\x01\x89\x00\x1C"
    TXT_BUTTPARENT_STR "\x00\x00"
    "\x00\x00\x01\x8C\x00\x06"
    TXT_NAME_STR "\x00\x00"
    "\x00\x00\x01\x8D\x00\x06"
    TXT_SIZE_STR "\x00\x00"
    "\x00\x00\x01\x8E\x00\x06"
    TXT_DATE_STR "\x00\x00"
    "\x00\x00\x01\x8F\x00\x06"
    TXT_BITS_STR "\x00\x00"
    "\x00\x00\x01\x90\x00\x08"
    TXT_BUTTDIR_STR "\x00\x00"
    "\x00\x00\x01\x91\x00\x0C"
    TXT_BUTTDIR2_STR "\x00"
    "\x00\x00\x01\x92\x00\x06"
    TXT_COPY_STR "\x00\x00"
    "\x00\x00\x01\x93\x00\x04"
    TXT_CUT_STR "\x00"
    "\x00\x00\x01\x94\x00\x06"
    TXT_EDIT_STR "\x00\x00"
    "\x00\x00\x01\x95\x00\x06"
    TXT_PASTE_STR "\x00"
    "\x00\x00\x01\x96\x00\x12"
    TXT_PATHNOTIFY_STR "\x00"
    "\x00\x00\x01\x97\x00\x08"
    TXT_SEARCH_STR "\x00\x00"
    "\x00\x00\x01\x98\x00\x0C"
    TXT_SELALL_STR "\x00\x00"
    "\x00\x00\x01\x99\x00\x10"
    TXT_SELNONE_STR "\x00\x00"
    "\x00\x00\x01\x9A\x00\x12"
    TXT_SELTOGGLE_STR "\x00\x00"
    "\x00\x00\x01\x9B\x00\x12"
    TXT_SELTYPE_STR "\x00\x00"
    "\x00\x00\x01\x9C\x00\x10"
    TXT_SELPATTERN_STR "\x00\x00"
    "\x00\x00\x01\x9D\x00\x08"
    TXT_DELETE_STR "\x00\x00"
    "\x00\x00\x01\x9E\x00\x08"
    TXT_COPYAS_STR "\x00"
    "\x00\x00\x01\x9F\x00\x06"
    TXT_FILE_STR "\x00\x00"
    "\x00\x00\x01\xA0\x00\x06"
    TXT_MOVE_STR "\x00\x00"
    "\x00\x00\x01\xA1\x00\x06"
    TXT_PACK_STR "\x00\x00"
    "\x00\x00\x01\xA2\x00\x14"
    TXT_BUTTREFRESH_STR "\x00"
    "\x00\x00\x01\xA3\x00\x10"
    TXT_CREATEARCHIVE_STR "\x00\x00"
    "\x00\x00\x01\xA4\x00\x14"
    TXT_CREATESUBDIR_STR "\x00"
    "\x00\x00\x01\xA5\x00\x2C"
    TXT_DELOKSINGLE_STR "\x00"
    "\x00\x00\x01\xA6\x00\x08"
    TXT_GENERIC_STR "\x00"
    "\x00\x00\x01\xA7\x00\x10"
    TXT_BUTTACTIVATE_STR "\x00"
    "\x00\x00\x01\xA8\x00\x12"
    TXT_BUTTDEACTIVATE_STR "\x00"
    "\x00\x00\x01\xAB\x00\x08"
    MSG_COMMENT_STR "\x00"
    "\x00\x00\x01\xAC\x00\x08"
    MSG_ARCHIVE_STR "\x00"
    "\x00\x00\x01\xAE\x00\x0C"
    MSG_EXECUTABLE_STR "\x00\x00"
    "\x00\x00\x01\xAF\x00\x0C"
    MSG_OPENWINDOW_STR "\x00"
    "\x00\x00\x01\xB0\x00\x0A"
    MSG_READABLE_STR "\x00\x00"
    "\x00\x00\x01\xB1\x00\x10"
    MSG_ROOTDIR_STR "\x00\x00"
    "\x00\x00\x01\xB2\x00\x08"
    MSG_SCRIPT_STR "\x00\x00"
    "\x00\x00\x01\xB3\x00\x08"
    MSG_VOLUMES_STR "\x00"
    "\x00\x00\x01\xB4\x00\x0A"
    MSG_WRITABLE_STR "\x00\x00"
    "\x00\x00\x01\xB5\x00\x0A"
    MSG_DELETABLE_STR "\x00"
    "\x00\x00\x01\xB6\x00\x0A"
    MSG_COMMENT8_STR "\x00\x00"
    "\x00\x00\x01\xB7\x00\x06"
    MSG_DATE8_STR "\x00"
    "\x00\x00\x01\xB8\x00\x18"
    MSG_DATENOTVALID_STR "\x00"
    "\x00\x00\x01\xB9\x00\x1C"
    MSG_FILEALREADYEXISTS_STR "\x00\x00"
    "\x00\x00\x01\xBA\x00\x06"
    MSG_NAME8_STR "\x00"
    "\x00\x00\x01\xBB\x00\x0C"
    MSG_PROTECTION8_STR "\x00"
    "\x00\x00\x01\xBC\x00\x08"
    MSG_CANCEL1_STR "\x00"
    "\x00\x00\x01\xBD\x00\x04"
    MSG_OK1_STR "\x00"
    "\x00\x00\x01\xBE\x00\x18"
    MSG_NOTRASHCANAVAILABLE_STR "\x00"
    "\x00\x00\x01\xC0\x00\x26"
    MSG_BUTTFUELGAUGE_STR "\x00"
    "\x00\x00\x01\xC1\x00\x14"
    MSG_COUNTINGFILES_STR "\x00\x00"
    "\x00\x00\x01\xC2\x00\x44"
    TXT_DELOKTRASH_STR "\x00\x00"
    "\x00\x00\x01\xC3\x00\x3C"
    TXT_DELOKSINGLETRASH_STR "\x00"
    "\x00\x00\x01\xC4\x00\x4E"
    MSG_DEPROTECTFILE_STR "\x00"
    "\x00\x00\x01\xC5\x00\x20"
    ERR_NOMKDIR_STR "\x00"
    "\x00\x00\x01\xC6\x00\x08"
    MSG_VERSION_STR "\x00"
    "\x00\x00\x01\xC7\x00\x0E"
    MSG_NOTAVAILABLE_STR "\x00"
    "\x00\x00\x01\xC8\x00\x0A"
    MSG_VERSION8_STR "\x00\x00"
    "\x00\x00\x01\xC9\x00\x10"
    MSG_COPYANDDELETE_STR "\x00"
    "\x00\x00\x01\xCA\x00\x0E"
    MSG_PULLDOWNMENU_STR "\x00"
    "\x00\x00\x01\xCB\x00\x18"
    MSG_REPLACEVERSION_STR "\x00\x00"
    "\x00\x00\x01\xCC\x00\x06"
    MSG_SAVE1_STR "\x00"
    "\x00\x00\x01\xCD\x00\x0E"
    MSG_SHOWVERSION_STR "\x00\x00"
    "\x00\x00\x01\xCE\x00\x0C"
    MSG_COUNTFILES_STR "\x00"
    "\x00\x00\x01\xD0\x00\x08"
    MSG_CREATE1_STR "\x00"
    "\x00\x00\x01\xD1\x00\x06"
    MSG_ICON_STR "\x00\x00"
    "\x00\x00\x01\xD2\x00\x18"
    MSG_NAMEOFDIR8_STR "\x00\x00"
    "\x00\x00\x01\xD3\x00\x2C"
    MSG_PLEASEENTERDIRNAME_STR "\x00"
    "\x00\x00\x01\xD6\x00\x1C"
    MSG_ASSIGNTITLE_STR "\x00\x00"
    "\x00\x00\x01\xD7\x00\x08"
    MSG_COPYTO_STR "\x00"
    "\x00\x00\x01\xD8\x00\x4A"
    MSG_ERR_ASSIGN_STR "\x00\x00"
    "\x00\x00\x01\xD9\x00\x0A"
    MSG_OPENWITH_STR "\x00"
    "\x00\x00\x01\xDA\x00\x06"
    MSG_PATHS_STR "\x00"
    "\x00\x00\x01\xDB\x00\x0E"
    MSG_VISITEDPATHS_STR "\x00"
    "\x00\x00\x01\xDC\x00\x1C"
    MSG_ASSIGNEXISTS_STR "\x00"
    "\x00\x00\x01\xDD\x00\x0C"
    MSG_ASSIGNMAKE_STR "\x00"
    "\x00\x00\x01\xDE\x00\x14"
    MSG_ASSIGNREPLACEADD_STR "\x00\x00"
    "\x00\x00\x01\xDF\x00\x0E"
    MSG_ASSIGNREMOVE_STR "\x00"
    "\x00\x00\x01\xE0\x00\x2A"
    MSG_ASSIGNWANTREMOVE_STR "\x00"
    "\x00\x00\x01\xE1\x00\x2A"
    MSG_CLOSEWINDOW_STR "\x00\x00"
    "\x00\x00\x01\xE2\x00\x44"
    MSG_CONFIRMCHECKSIZE_STR "\x00\x00"
    "\x00\x00\x01\xE3\x00\x20"
    MSG_DDIRSDFILES_STR "\x00\x00"
    "\x00\x00\x01\xE4\x00\x16"
    MSG_NORENAME_STR "\x00"
    "\x00\x00\x01\xE5\x00\xAA"
    ERR_WRONGPREFS_STR "\x00\x00"
    "\x00\x00\x01\xE6\x00\x10"
    MSG_CONFIRMDELETE_STR "\x00\x00"
    "\x00\x00\x01\xE7\x00\x06"
    MSG_TOOLS_STR "\x00"
    "\x00\x00\x01\xE8\x00\x36"
    MSG_NOCOPYTOSUBDIR_STR "\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};


#ifdef CATCOMP_CODE

STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);

#undef LocaleBase
#define LocaleBase li->li_LocaleBase
    
    if (LocaleBase)
        return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#undef LocaleBase

    return(builtIn);
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* CLASSACTIONCD_H */
