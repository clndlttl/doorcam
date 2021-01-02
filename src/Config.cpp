#include <Config.h>

Config::Config(const std::string& filename) {		 
    std::lock_guard<std::mutex> lock(mtx);
    boost::property_tree::json_parser::read_json(
           filename, cfg);

    // get mode from json spec, default to server mode
    std::string modestr = cfg.get("mode", "server");
    
    if (modeCode.find(modestr) != modeCode.end()) {
      mode = modeCode[modestr];
    }
}

int Config::getImageWidth() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::stoi( cfg.get("imgWidth", "320") );
}

int Config::getImageHeight() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::stoi( cfg.get("imgHeight", "240") );
}

int Config::getMode() const {
    std::lock_guard<std::mutex> lock(mtx);
    return mode;
}

float Config::getFPS() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::stof( cfg.get("fps", "10.0") );
}

void Config::quit() {
    std::lock_guard<std::mutex> lock(mtx);
    mode = QUIT;
}

bool Config::isTimeToQuit() { 
    std::lock_guard<std::mutex> lock(mtx);
    return mode == QUIT; 
}

void Config::setMode(Mode m) {
    std::lock_guard<std::mutex> lock(mtx);
    mode = m; 
}
