#!/bin/bash
#####
# BarebonesAndroid
#  v1.0
#  Android /system/app backup & trim
#
# Justin Swanson - www.faecbawks.com
#
# CHANGELOG:
#  1.0 - 04/09/11
#   - Initial release
#  1.1 - 04/23/11
#   - Added offers for Google apps added by the 'gapps.zip' CM addon
#   - Added prompts for generalized categories, allowing blocks of offers to be skipped
#
# TODO:
# - Add support for multiple devices.
#  - Add device serial number to commands.
#  - "adb -s 2011040131302008 [push/pull/shell/etc]
# - Add more error checking and failproofing.
# - Streamline the experience.
# - Sort apps. Add app Categories.
#
#####
# Offer function
# -Offers the user to uninstall a certain package.
# -Written as a way to avoid re-writing the same if-else statement over and over again
# Takes four/six arguements
# $1 - App name
# $2 - App notes
# $3 - Package's .apk filename
# $4 - Package's system name
# $5 - 2nd Package's .apk filename (OPTIONAL)
# $6 - 2nd Package's system name (OPTIONAL)
offer ()
{
	# Ask the user if they wish to remove the proposed App
	echo -n -e "\nDo you want to remove $1?"
	# IF this app has a note with it, display it,
	if [ -n "$2" ]
	then
		echo -n -e "\nNote: $2"
	fi

	# Prompt for user input
	echo -n " Y or [N]: "
	read INPUT

	# Test $INPUT for user response and respond accordingly
	case "$INPUT" in
		# if Y/y, call the uninstall function
		y|Y)
			echo "Uninstalling $1..."
			# Calling rmPkg() on the App's package info
			rmPkg $3 $4
			# If a second package is included, remove that too
			if [ -n "$5" ]
			then
				rmPkg $5 $6
			fi
		;;
		# if ANYTHING other than Y/y is entered, skip the uninstall
		*)
			echo "Skipping $1..."
		;;
	esac
}
#
# Package Uninstall Function
rmPkg ()
{
	echo "Removing $1..."
	adb shell rm -f /system/app/$1
#	echo "adb shell rm -f /system/app/$1"
	if [ -n "$2" ]
	then
		adb uninstall $2
#		echo "adb uninstall $2"
	fi
}
#
# MAIN PROGRAM
#
# START device check
# Check to make sure the device is connected (AND IT IS THE ONLY DEVICE CONNECTED! (This will not be an issue in the future))
echo -e "Before we begin, we must test if your device is connected and it is the _ONLY_ device connected! If no devices are connected this script will not function properly, and if multiple devices are connected you might alter the wrong device.\n"
adb devices
echo -n -e "Were any devices listed? Y or [N]: ";
read RESULT
if [ "$RESULT" = "y" -o "$RESULT" = "Y" ]
then
	echo -e "Good! We shall proceed.\n"
else
	echo -e "\nPlease connect your device and try again.\n"
	exit
fi
# END device check
#
# START /system/app backup procedure
echo -n "Do you want to make a backup of your system packages? (RECCOMENDED) Y or [N]: "
read ANSWER
case "$ANSWER" in
	# if Y/y, run backup procedure
	y|Y)
		echo -e "\nEnter the name of the folder for the backup files."
		echo "If it does not exist it will be created."
		echo "If left blank, the default folder will be [./apkBackup]"
		echo "NOTE: The tilde [~] will NOT resolve to the /home/ folder. Sorry!"
		echo -n "Enter your selection: "
		read FOLDER
		# set default folder if nothing is entered
		if [ -z $FOLDER ]; then
			FOLDER="./apkBackup"
		fi

		# run the backup command
		echo "Backing up files to [$FOLDER]..."
		adb pull /system/app/ $FOLDER/
		echo -e "\nSystem packages have been backed up!\n"
	;;
	# if ANYTHING other than Y/y is entered, do not run the backup
	*)
		echo -e "Your system packages will not be backed up.\n"
	;;
esac
# END backup procedure
#
# START /system/app trimming procedure
echo -n "Do you want to do a step-by-step removal of system packages? Y or [N]: "
read ANSWER
case "$ANSWER" in
	# if Y/y, run trimming procedure
	y|Y)
		# Remount the Android filesystem as read/write
		echo "Remounting the device's filesystem as read/write..."
		adb remount

		echo -n "Would you like to take a look at the CyanogenMod specific packages? Y or [N]: "
		read ANSWER
		case "$ANSWER" in
			# if Y/y, offer CyanogenMod specific packages
			y|Y)
				#__CYANOGENMOD_ONLY__
				# CyanogenMod Stats
				offer "CM Stats" "CyanogenMod anonymous statistics reporting service. Safe to remove, but will FC CMParts (CyanogenMod Settings) if you try and select the configuration options if it's not installed." "CMStats.apk" "com.cyanogenmod.stats"
				# CyanogenMod Update Notifications
				offer "CM Update Notifcations" "CyanogenMod Update Notifications. Safe to remove, but will FC CMParts (CyanogenMod Settings) if you try and select the update configuration options if it's not installed." "CMUpdateNotify.apk"
				# CyanogenMod Wallpapers
				offer "CyanogenMod Wallpapers" "Default CyanogenMod wallpapers." "CMWallpapers.apk" "com.cyanogenmod.CMWallpapers"
			;;
			# if ANYTHING other than Y/y is entered, skip CM packages
			*)
				echo -e "Skipping CyanogenMod packages...\n"
			;;
		esac

		echo -n "Did you install the Google Apps addon for CyanogenMod? (gapps*.zip) Y or [N]: "
		read ANSWER
		case "$ANSWER" in
			# if Y/y, offer Google Apps specific packages
			y|Y)
				# CarHome app
				offer "CarHome Launcher" "Not needed. Safe to remove." "CarHomeGoogle.apk" "com.google.android.carhome"
				# Google Android Market
				offer "Android Market" "Android Marketplace app." "Vending.apk" "com.android.vending" "MarketUpdater.apk" "com.android.vending.updater"
				# Google Books
				offer "Google Books" "Google Books app. Used for reading eBooks from the Google Books service. Safe to remove." "BooksPhone.apk" "com.google.android.apps.books"
				# Google Talk
				offer "GoogleTalk" "Google's instant messaging client for the GTalk service" "Talk.apk" "com.google.android.talk"
				# Genie Widget
				offer "Genie Widget" "News and Weather Widget. Will remove weather information on the DeskClock app. Safe to remove." "GenieWidget.apk" "com.google.android.apps.genie.geniewidget"
				# OneTimeInitializer
				offer "One Time Initializer" "??? Seemingly useless. Safe to remove." "OneTimeInitializer.apk" "com.google.android.onetimeinitializer"
				# On Screen Keyboard tutorial
				offer "On-Screen Keyboard tutorial" "Tutorial for on-screen keyboard. Safe to remove." "LatinImeTutorial.apk" "com.google.android.latinimetutorial"
				# Setup Wizard
				offer "Setup Wizard" "Setup Wizard that runs on first boot. Safe to remove" "SetupWizard.apk" "com.android.setupwizard"
			;;
			# if ANYTHING other than Y/y is entered, skip GAPPS packages
			*)
				echo -e "Skipping Google Apps packages...\n"
			;;
		esac

		echo -n "Would you like to take a look at all the rest of the general apps? Y or [N]: "
		read ANSWER
		case "$ANSWER" in
			# if Y/y, offer general apps
			y|Y)
				# ADWLauncher
				offer "ADW Launcher" "Only remove if you have a second replacement home." "ADWLauncher.apk" "com.android.launcher"
				# Android Terminal
				offer "Android Terminal" "Android terminal emulator. Gives shell access from your phone. Safe to remove." "AndroidTerm.apk" "jackpal.androidterm2"
				# Bluetooth
				offer "Bluetooth" "Will remove bluetooth functionality. Not needed if you do not use it." "Bluetooth.apk" "com.android.bluetooth"
				# Browser
				offer "Browser" "Stock internet browser. Safe to remove." "Browser.apk" "com.android.browser"
				# Calculator
				offer "Calculator" "Stock calculator app." "Calculator.apk" "com.android.calculator2"
				# Calendar
				offer "Calendar" "Stock Calendar app / widget. Safe to remove." "Calendar.apk" "com.android.calendar"
				# Calendar Sync Service
				offer "Calendar Sync Service" "Remove this if you do not want Calendar Sync with Google Calendar. Safe to remove." "CalendarProvider.apk" "com.android.providers.calendar"
				# Camera app
				offer "Camera" "Not needed if your device doesn't have a camera." "Camera.apk" "com.android.camera"
				# Contacts
				offer "Contacts" "Contacts app. Includes Contacts, Call Log, Favorite Conctacts, and Phone Dialer. DO NOT remove on a phone device!" "Contacts.apk" "com.android.contacts" "ContactsProvider.apk" "com.android.providers.contacts"
				# Desk Clock
				offer "Desk Clock" "Clock and Alarm app. Safe to remove." "DeskClock.apk" "com.android.deskclock"
				# Development app
				offer "Development App" " Safe to remove." "Development.apk" "com.android.development"
				# DSP Manager
				offer "DSP Manager" "Digital Sound Processing. Sound equalizer app. Safe to remove." "DSPManager.apk" "com.bel.android.dspmanager"
				# E-Mail
				offer "E-Mail" "POP/IMAP e-mail client. Seperate from the GMail app. Safe to remove." "Email.apk" "com.android.email"
				# FileManager
				offer "File Manager" "IOFileManager. Safe to remove." "FileManager.apk" "org.openintents.filemanager"
				# 3D Gallery
				offer "3D Gallery" "3D Photo Gallery. Safe to remove." "Gallery3D.apk" "com.cooliris.media"
				# HTML Viewer
				offer "HTML Viewer" "Doesn't seem to be needed. Safe to remove." "HTMLViewer.apk" "com.android.htmlviewer"
				# On Screen Keyboard
				offer "On-Screen Keyboard" "Soft-Keyboard and its dictionary. Safe to remove if you have a physical keyboard or a replacement software keyboard." "LatinIME.apk" "com.android.inputmethod.latin" "UserDictionaryProvider.apk" "com.android.providers.userdictionary"
				#
				# LiveWallpapers #
				# LiveWallpaper Picker
				offer "Live Wallpaper Selector" "Interface for setting LiveWallpapers. You will lose the ability to use LiveWallpapers. Safe to remove." "LiveWallpapersPicker.apk" "com.android.wallpaper.livepicker"
				# Live Wallpaper packages
				offer "LiveWallpapers - base package" "An assortment of pre-installed LiveWallpapers. Will _NOT_ remove your ability to use LiveWallpapers. Safe to remove." "LiveWallpapers.apk" "com.android.wallpaper"
				offer "LiveWallpapers - Visualizations" "Music visualization based LiveWallpapers. Will _NOT_ remove your ability to use LiveWallpapers. Safe to remove." "VisualizationWallpapers.apk" "com.android.musicvis"
				offer "LiveWallpapers - MagicSmoke" "MagicSmoke LiveWallpaper. Will _NOT_ remove your ability to use LiveWallpapers. Safe to remove." "MagicSmokeWallpapers.apk" "com.android.magicsmoke"
				#
				# MMS
				offer "SMS/MMS" "Provides SMS/MMS services." "Mms.apk" "com.android.mms"
				# Music
				offer "Music Player" "Music and Video player. Safe to remove." "Music.apk" "com.android.music"
				# ProTips Widget
				offer "ProTips Widget" "ProTips home widget. Safe to remove." "Protips.apk" "com.android.protips"
				# PacMan
				offer "PacMan Package" "Allows you to download Google apps on first boot. No longer needed after first boot. Safe to remove." "Pacman.apk" "com.teamdouche.pacman"
				# Rom Manager
				offer "ROM Manager" "Built-in installation of ROM Manager. Updated versions can be re-installed from Android Market. Safe to remove" "RomManager.apk" "com.koushikdutta.rommanager"
				# Sound Recorder
				offer "Sound Recorder" "Safe to remove." "SoundRecorder.apk" "com.android.soundrecorder"
				# Spare Parts
				offer "Spare Parts" "Extra 'settings' menu. Safe to remove." "SpareParts.apk" "com.android.spare_parts"
				# TextToSpeech
				offer "Text-to-Speech services" "Needed by Google Navigator. Otherwise safe to remove." "TtsService.apk" "android.tts" "PicoTts.apk" "com.svox.pico"
				#
				# Themes #
				# Theme engine
				offer "Interface Theme Engine" "Will remove the theme picker and your ability to utilize custom interface themes. Safe to remove." "ThemeManager.apk" "com.tmobile.thememanager" "ThemeChooser.apk" "com.tmobile.themechooser"
				# Themes
				offer "Interface Themes" "Non-default interface themes. Safe to remove." "Androidian.apk" "com.tmobile.theme.Androidian" "Cyanbread.apk" "com.tmobile.theme.Cyanbread"
				#
				# Voice Dialer
				offer "Voice Dialer" "Voice dialing app. Safe to remove." "VoiceDialer.apk" "com.android.voicedialer"
				# VPN Services
				offer "VPN Services" "Virtual Private Network app. Safe to remove." "VpnServices.apk" "com.android.server.vpn"
				#
				# Phone Service #
				offer "Telephony services" "Needed for telephony capabilities, but safe to remove if your device has no phone capability (i.e. Nook Color). WILL force-close on 'Settings -> About Tablet -> Status' screen if removed" "Phone.apk" "com.android.phone" "TelephonyProvider.apk" "com.android.providers.telephony"
			;;
			# if ANYTHING other than Y/y is entered, skip general apps
			*)
				echo -e "Skipping general apps...\n"
			;;
		esac


		echo -e "\nThe system trimming procedure has finished! Your device will now reboot.\n"

		adb reboot
	;;
	# if ANYTHING other than Y/y is entered, do not run the trimming
	*)
		echo -e "Your system packages will not be trimmed.\n"
	;;
esac
# END trimming procedure
