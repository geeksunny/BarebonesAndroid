//============================================================================
// Name        : BarebonesAndroid.cpp
// Author      : Justin Swanson
// Website     : www.faecbawks.com
// Version     : 1.1
// Copyright   : Open-Source ftw?
// Description : A port of my BASH script in C++
//============================================================================

// cout, cin
#include <iostream>
// system()
#include <stdlib.h>
// string abilities
#include <string>
using namespace std;

// Package uninstall function
// rmPkg( APP FILENAME, APP PACKAGE NAME)
void rmPkg(string apk, string pkg)
{
	cout << "Removing " << apk << "..." << endl;

	// Create the command string
	string COMMAND = "adb shell rm -f /system/app/"+apk;
	// Convert command string to [const char*] type for system() to utilize
	const char* COMMAND2 = COMMAND.c_str();
	// Run command string
	system(COMMAND2);

	// Uninstall package name ONLY if provided
	if (pkg != "")
	{
		COMMAND = "adb uninstall "+pkg;
		COMMAND2 = COMMAND.c_str();
		system(COMMAND2);
	}
}

// Offer function
// -Offers the user to uninstall a certain package.
// -Written as a way to avoid re-writing the same if-else statment over and over again
// There are two versions of this function.
// Four variables or Six variables
// string appName - Proposed app's name
// string appNote - Proposed app's note
// string pkgApk1 - The package's .apk filename
// string pkgName1 - The package's system name (for package manager uninstall call)
// string pkgApk2 - The package's second .apk filename (OPTIONAL)
// string pkgName2 - The package's second system name (OPTIONAL)
// -Four variables version-
int offer(string appName, string appNote, string pkgApk1, string pkgName1)
{
	// Ask the user if they wish to remove the proposed App
	cout << "Do you want to remove " << appName << "?";
	// IF this app has a note with it, display it.
	if (appNote != "")
		cout << endl << "Note: " << appNote;

	// Prompt for user input
	cout << " Y or N: ";
	string INPUT;
	cin >> INPUT;
	if (INPUT[0] == 'y' || INPUT[0] == 'Y')
	{
		cout << "Uninstalling " << appName << "..." << endl;
		// calling rmPkg() on the App's package info
		rmPkg(pkgApk1,pkgName1);
		// Return 0 for removed package
		return 0;
	}
	else
		cout << "Skipping " << appName << "..." << endl;
	// Return 1 for skipped package
	return 1;
}
// -Six variables version-
// Calls on four variables version
// If user says YES to removal, will call rmPkg() on extra two variables
void offer(string appName, string appNote, string pkgApk1, string pkgName1, string pkgApk2, string pkgName2)
{
	int result = offer(appName,appNote,pkgApk1,pkgName1);
	if (result == 0)
		rmPkg(pkgApk2,pkgName2);
}

// MAIN PROGRAM
int main() {
	/// START device check
	// Check to make sure the device is connected (AND IT IS THE ONLY DEVICE CONNECTED! (This will not be an issue in the future))
	cout << "Before we begin, we must test if your device is connected and it is the _ONLY_ device connected! If no devices are connected this script will not function properly, and if multiple devices are connected you might alter the wrong device." << endl << endl;
	system("adb devices");
	cout << "Were any devices listed? Y or N: ";
	// Using STRINGS for input to prevent user from sending multiple answers at once.
	// if CHAR and user enters "yyy", it would answer 'yes' as this prompt, plus two more beyond it.
	// Need to find a better alternative. One that would also assume [ENTER] as a "NO" answer.
	// cin.get(); provided unpredictable results
	string ANSWER;
	cin >> ANSWER;
	if (ANSWER[0] == 'y' || ANSWER[0] == 'Y')
		cout << "Good! We shall proceed." << endl;
	else
	{
		cout << endl << "Please connect your device and try again." << endl;
		return 0;
	}
	/// END device check
	//
	/// START /system/app backup procedure
	cout << "Do you want to make a backup of your system packages? (RECCOMENDED) Y or N: ";
	// re-using ANSWER from first user-check
	cin >> ANSWER;
	if (ANSWER[0] == 'y' || ANSWER[0] == 'Y')
	{
		cout << endl
				<< "Enter the name of the folder for the backup files." << endl
				<< "If it does not exist it will be created." << endl
				<< "The folder will be relative to the directory you are running this program from." << endl
				//<< "If left blank, the default folder will be [./apkBackup]" << endl
				// Default inputs do not work yet...
				<< "Enter your selection: ";
		string FOLDER;
		cin >> FOLDER;

		cout << "Backing up files to [" << FOLDER << "]...";
		// Create the command string
		string COMMAND = "adb pull /system/app/ "+FOLDER+"/";
		// Convert command string to [const char*] type for system() to utilize
		const char* COMMAND2 = COMMAND.c_str();
		// Run command string
		system(COMMAND2);
		cout << endl << "System Packages have been backed up!" << endl;
	}
	else
	{
		cout << endl << "Your system packages will not be backed up." << endl;
	}
	/// END backup procedure
	//
	/// START /system/app trimming procedure
	cout << "Do you want to do a step-by-step removal of system packages? Y or N: ";
	// re-using ANSWER from first user-check
	cin >> ANSWER;
	if (ANSWER[0] == 'y' || ANSWER[0] == 'Y')
	{
		// Remount the Android filesystem as read/write
		system("adb remount");

		/// offer CyanogenMod specific packages
		cout << "Would you like to take a look at the CyanogenMod specific packages? Y or N: ";
		// re-using ANSWER from first user-check
		cin >> ANSWER;
		if (ANSWER[0] == 'y' || ANSWER[0] == 'Y')
		{
			// CyanogenMod Stats
			offer("CM Stats","CyanogenMod anonymous statistics reporting service. Safe to remove, but will FC CMParts (CyanogenMod Settings) if you try and select the configuration options if it's not installed.","CMStats.apk","com.cyanogenmod.stats");
			// CyanogenMod Update Notifications
			offer("CM Update Notifcations","CyanogenMod Update Notifications. Safe to remove, but will FC CMParts (CyanogenMod Settings) if you try and select the update configuration options if it's not installed.","CMUpdateNotify.apk","");
			// CyanogenMod Wallpapers
			offer("CyanogenMod Wallpapers","Default CyanogenMod wallpapers.","CMWallpapers.apk","com.cyanogenmod.CMWallpapers");
		}
		else
		{
			cout << "Skipping CyanogenMod packages..." << endl;
		}

		/// offer Google Apps specific packages
		cout << "Did you install the Google Apps addon for CyanogenMod? (gapps*.zip) Y or N: ";
		// re-using ANSWER from first user-check
		cin >> ANSWER;
		if (ANSWER[0] == 'y' || ANSWER[0] == 'Y')
		{
			// CarHome app
			offer("CarHome Launcher","Not needed. Safe to remove.","CarHomeGoogle.apk","com.google.android.carhome");
			// Google Android Market
			offer("Android Market","Android Marketplace app.","Vending.apk","com.android.vending","MarketUpdater.apk","com.android.vending.updater");
			// Google Books
			offer("Google Books","Google Books app. Used for reading eBooks from the Google Books service. Safe to remove.","BooksPhone.apk","com.google.android.apps.books");
			// Google Talk
			offer("GoogleTalk","Google's instant messaging client for the GTalk service","Talk.apk","com.google.android.talk");
			// Genie Widget
			offer("Genie Widget","News and Weather Widget. Will remove weather information on the DeskClock app. Safe to remove.","GenieWidget.apk","com.google.android.apps.genie.geniewidget");
			// OneTimeInitializer
			offer("One Time Initializer","??? Seemingly useless. Safe to remove.","OneTimeInitializer.apk","com.google.android.onetimeinitializer");
			// On Screen Keyboard tutorial
			offer("On-Screen Keyboard tutorial","Tutorial for on-screen keyboard. Safe to remove.","LatinImeTutorial.apk","com.google.android.latinimetutorial");
			// Setup Wizard
			offer("Setup Wizard","Setup Wizard that runs on first boot. Safe to remove","SetupWizard.apk","com.android.setupwizard");
		}
		else
		{
			cout << "Skipping Google Apps packages..." << endl;
		}

		/// offer general apps
		cout << "Would you like to take a look at all the rest of the general apps? Y or N: ";
		// re-using ANSWER from first user-check
		cin >> ANSWER;
		if (ANSWER[0] == 'y' || ANSWER[0] == 'Y')
		{
			// ADWLauncher
			offer("ADW Launcher","Only remove if you have a second replacement home.","ADWLauncher.apk","com.android.launcher");
			// Android Terminal
			offer("Android Terminal","Android terminal emulator. Gives shell access from your phone. Safe to remove.","AndroidTerm.apk","jackpal.androidterm2");
			// Bluetooth
			offer("Bluetooth","Will remove bluetooth functionality. Not needed if you do not use it.","Bluetooth.apk","com.android.bluetooth");
			// Browser
			offer("Browser","Stock internet browser. Safe to remove.","Browser.apk","com.android.browser");
			// Calculator
			offer("Calculator","Stock calculator app.","Calculator.apk","com.android.calculator2");
			// Calendar
			offer("Calendar","Stock Calendar app / widget. Safe to remove.","Calendar.apk","com.android.calendar");
			// Calendar Sync Service
			offer("Calendar Sync Service","Remove this if you do not want Calendar Sync with Google Calendar. Safe to remove.","CalendarProvider.apk","com.android.providers.calendar");
			// Camera app
			offer("Camera","Not needed if your device doesn't have a camera.","Camera.apk","com.android.camera");
			// Contacts
			offer("Contacts","Contacts app. Includes Contacts, Call Log, Favorite Conctacts, and Phone Dialer. DO NOT remove on a phone device!","Contacts.apk","com.android.contacts","ContactsProvider.apk","com.android.providers.contacts");
			// Desk Clock
			offer("Desk Clock","Clock and Alarm app. Safe to remove.","DeskClock.apk","com.android.deskclock");
			// Development app
			offer("Development App","Safe to remove.","Development.apk","com.android.development");
			// DSP Manager
			offer("DSP Manager","Digital Sound Processing. Sound equalizer app. Safe to remove.","DSPManager.apk","com.bel.android.dspmanager");
			// E-Mail
			offer("E-Mail","POP/IMAP e-mail client. Seperate from the GMail app. Safe to remove.","Email.apk","com.android.email");
			// FileManager
			offer("File Manager","IOFileManager. Safe to remove.","FileManager.apk","org.openintents.filemanager");
			// 3D Gallery
			offer("3D Gallery","3D Photo Gallery. Safe to remove.","Gallery3D.apk","com.cooliris.media");
			// HTML Viewer
			offer("HTML Viewer","Doesn't seem to be needed. Safe to remove.","HTMLViewer.apk","com.android.htmlviewer");
			// On Screen Keyboard
			offer("On-Screen Keyboard","Soft-Keyboard and its dictionary. Safe to remove if you have a physical keyboard or a replacement software keyboard.","LatinIME.apk","com.android.inputmethod.latin","UserDictionaryProvider.apk","com.android.providers.userdictionary");
			//
			// LiveWallpapers //
			// LiveWallpaper Picker
			offer("Live Wallpaper Selector","Interface for setting LiveWallpapers. You will lose the ability to use LiveWallpapers. Safe to remove.","LiveWallpapersPicker.apk","com.android.wallpaper.livepicker");
			// Live Wallpaper packages
			offer("LiveWallpapers - base package","An assortment of pre-installed LiveWallpapers. Will _NOT_ remove your ability to use LiveWallpapers. Safe to remove.","LiveWallpapers.apk","com.android.wallpaper");
			offer("LiveWallpapers - Visualizations","Music visualization based LiveWallpapers. Will _NOT_ remove your ability to use LiveWallpapers. Safe to remove.","VisualizationWallpapers.apk","com.android.musicvis");
			offer("LiveWallpapers - MagicSmoke","MagicSmoke LiveWallpaper. Will _NOT_ remove your ability to use LiveWallpapers. Safe to remove.","MagicSmokeWallpapers.apk","com.android.magicsmoke");
			//
			// MMS
			offer("SMS/MMS","Provides SMS/MMS services.","Mms.apk","com.android.mms");
			// Music
			offer("Music Player","Music and Video player. Safe to remove.","Music.apk","com.android.music");
			// ProTips Widget
			offer("ProTips Widget","ProTips home widget. Safe to remove.","Protips.apk","com.android.protips");
			// PacMan
			offer("PacMan Package","Allows you to download Google apps on first boot. No longer needed after first boot. Safe to remove.","Pacman.apk","com.teamdouche.pacman");
			// Rom Manager
			offer("ROM Manager","Built-in installation of ROM Manager. Safe to remove","RomManager.apk","com.koushikdutta.rommanager");
			// Sound Recorder
			offer("Sound Recorder","Safe to remove.","SoundRecorder.apk","com.android.soundrecorder");
			// Spare Parts
			offer("Spare Parts","Extra 'settings' menu. Safe to remove.","SpareParts.apk","com.android.spare_parts");
			// TextToSpeech
			offer("Text-to-Speech services","Needed by Google Navigator. Otherwise safe to remove.","TtsService.apk","android.tts","PicoTts.apk","com.svox.pico");
			//
			// Themes //
			// Theme engine
			offer("Interface Theme Engine","Will remove the theme picker and your ability to utilize custom interface themes. Safe to remove.","ThemeManager.apk","com.tmobile.thememanager","ThemeChooser.apk","com.tmobile.themechooser");
			// Themes
			offer("Interface Themes","Non-default interface themes. Safe to remove.","Androidian.apk","com.tmobile.theme.Androidian","Cyanbread.apk","com.tmobile.theme.Cyanbread");
			//
			// Voice Dialer
			offer("Voice Dialer","Voice dialing app. Safe to remove.","VoiceDialer.apk","com.android.voicedialer");
			// VPN Services
			offer("VPN Services","Virtual Private Network app. Safe to remove.","VpnServices.apk","com.android.server.vpn");
			//
			// Phone Service //
			offer("Telephony services","Needed for telephony capabilities, but safe to remove if your device has no phone capability (i.e. Nook Color). WILL force-close on 'Settings -> About Tablet -> Status' screen if removed","Phone.apk","com.android.phone","TelephonyProvider.apk","com.android.providers.telephony");
		}
		else
		{
			cout << "Skipping general apps..." << endl;
		}


		cout << endl << "The system trimming procedure has finished! Your device will now reboot." << endl;
		system("adb reboot");
	}
	else
	{
		cout << "Your system packages will not be trimmed" << endl;
	}
	/// END trimming procedure

	return 0;
}