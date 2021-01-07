#include <Console.h>
#include <boost/algorithm/string.hpp>

ServerConsole::ServerConsole(Config* _cfg) : cfg(_cfg) {
  funcMap["quit"]   = &ServerConsole::quit;
  funcMap["server"] = &ServerConsole::setServerMode;
  funcMap["motion"] = &ServerConsole::setMotionMode;
  funcMap["size"]   = &ServerConsole::setResolution;
}

void ServerConsole::handle(const std::string& raw) {
  std::vector<std::string> args;
  boost::split(args, raw, boost::is_any_of("(,)"));

  std::string func = args.front();
  args.erase(args.begin()); 

  if (funcMap.find(func) != funcMap.end()) {
    (this->*funcMap[func])(args);
  } else {
    std::cout << "unknown command: " << func << std::endl;
  }
}

void ServerConsole::quit(Args a) { cfg->quit(); }
void ServerConsole::setServerMode(Args a) { cfg->setMode(::SERVER); }
void ServerConsole::setMotionMode(Args a) { cfg->setMode(::MOTION); }
void ServerConsole::setResolution(Args a) {
  if (a.size() >= 2) {
    cfg->setImageWidth( a[0] );
    cfg->setImageHeight( a[1] );
    cfg->update( RESOLUTION );
  } else {
    std::cout << "provide width and height, i.e. size:320:240" << std::endl;
  }
}	

