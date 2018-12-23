
#include <Application.h>


int main(int argc, char** argv)
{
	int exitCode = 0;
	Application::init(argc, argv, exitCode);
	exitCode = Application::run();
	Application::cleanUp();
	return exitCode;
}