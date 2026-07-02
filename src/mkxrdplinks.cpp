#include <iostream>
#include <CLI/CLI.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>

int main(int argc, char* argv[]){
	if (argc > 1){
		uid_t uid = getuid();
		passwd* pw = getpwuid(uid);

		if (!pw){
			std::cerr << "Failed to retrieve user information\n";
			return 1;
		}

		if (!std::filesystem::exists(pw->pw_dir)){
			std::cerr << "Failed to find user home directory";
			return 1;
		}

		std::filesystem::path userBinDir {pw->pw_dir + std::string("/bin")};
		std::filesystem::path userDesktopDir {pw->pw_dir + std::string("/Desktop")};

		if (!std::filesystem::exists(userBinDir)){
			if (!std::filesystem::create_directory(userBinDir)){
				std::cerr << "Unable to create user bin directory";
				return 1;
			}
		}

		if (!std::filesystem::exists(userDesktopDir)){
			if (!std::filesystem::create_directory(userDesktopDir)){
				std::cerr << "Failed to create user Desktop directory";
				return 1;
			}
		}

		CLI::App app{"mkxrdplinks", "Create xrdp desktop links"};

		std::string host;
		std::string domain;
		std::string username;
		std::string pass;
		std::string linkname;
		std::string comment;
		std::string iconpath{"computer"};
		bool cert{false};
		bool dynres{false};
		bool floatbar{false};
		bool disp{false};
		bool clipboard{false};
		bool multimon{false};
		bool sound{false};

		app.add_option("-v,--host,--ip", host, "Host-IP Address");
		app.add_option("-d,--domain", domain, "User login domain");
		app.add_option("-u,--username, --user", username, "User login name");
		app.add_option("-p, --password", pass, "User login password");
		app.add_option("-n, --name", linkname, "Desktop shortcut label");
		app.add_option("-t, --comment, --tooltip", comment, "Desktop shortcut tooltip");
		app.add_flag("-c,--cert", cert, "Report certificate errors");
		app.add_flag("-s,--disp", disp, "Disable client display settings");
		app.add_flag("-f,--floatbar", floatbar, "Disable floating control bar");
		app.add_flag("-r,--dynamic-resolution", dynres, "Disable dynamic resolution");
		app.add_flag("-l,--clipboard", disp, "Disable clipboard in client");
		app.add_flag("-o,--sound", multimon, "Disable audio streaming support");
		app.add_flag("-m,--multimonitor", multimon, "Enable multi-monitor support");
		app.add_option("-i, --icon", iconpath, "Desktop icon file path");

		CLI11_PARSE(app, argc, argv);

		std::string binFileName {userBinDir / std::string("xfreerdp-" + linkname + ".sh")};
		std::ofstream binFile;
		binFile.open(binFileName);
		if (!binFile){
			std::cerr << "Unable to create shell script file";
			return 1;
		}

		binFile << "#!/bin/bash\nxfreerdp \\\n";
		binFile << "/v:" + host + " \\\n";
		if (domain != ""){
			binFile << "/d:" << domain << " \\\n";
		}
		binFile << "/u:" << username << " \\\n";
		if (!cert){
			binFile << "/cert:ignore \\\n";
		}
		if (!floatbar){
			binFile << "/floatbar:sticky:on \\\n";
		}
		if (!dynres){
			binFile << "/dynamic-resolution \\\n";
		}
		if (pass != ""){
			binFile << "/p:" << pass << " \\\n";
		}
		if (!disp){
			binFile <<"/disp \\\n";
		}
		if (multimon){
			binFile <<"/multimon \\\n";
		}
		if (!sound){
			binFile << "/sound \\\n";
		}
		if (!clipboard){
			binFile << "+clipboard";
		}

		binFile.close();
		std::filesystem::permissions(binFileName, std::filesystem::perms::owner_read | std::filesystem::perms::owner_write | std::filesystem::perms::owner_exec);

		std::ofstream desktopFile;
		desktopFile.open(userDesktopDir / std::string("rdp-" + linkname + ".desktop"));
		if (desktopFile){
			desktopFile << "[Desktop Entry]\nVersion=1.0\nType=Application\nTerminal=false\nCategories=Network";
			if (linkname != ""){
				desktopFile << "\nName=" + linkname ;
			}
			if (comment != ""){
				desktopFile <<"\nComment=" + comment;
			}
			if (iconpath != ""){
				desktopFile << "\nIcon=" + iconpath;
			}
			desktopFile << "\nExec=" << binFileName;
			desktopFile.close();
		}
		else{
			std::cerr << "Unable to create user desktop file";
			return 1;
		}
	}
	else{
		std::cout << "Usage: to be continued...\n";
	}

	return 0;
}
