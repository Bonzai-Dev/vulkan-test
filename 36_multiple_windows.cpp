/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, strings, and string streams
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <sstream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Total windows
const int TOTAL_WINDOWS = 3;

class LWindow
{
	public:
		//Intializes internals
		LWindow();

		//Creates window
		bool init();

		//Handles window events
		void handleEvent( SDL_Event& e );

		//Focuses on window
		void focus();

		//Shows windows contents
		void render();

		//Deallocates internals
		void free();

		//Window dimensions
		int getWidth();
		int getHeight();

		//Window focii
		bool hasMouseFocus();
		bool hasKeyboardFocus();
		bool isMinimized();
		bool isShown();

	private:
		//Window data
		SDL_Window* mWindow;
		SDL_Renderer* mRenderer;
		int mWindowID;

		//Window dimensions
		int mWidth;
		int mHeight;

		//Window focus
		bool mMouseFocus;
		bool mKeyboardFocus;
		bool mFullScreen;
		bool mMinimized;
		bool mShown;
};

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//Our custom windows
LWindow gWindows[ TOTAL_WINDOWS ];

LWindow::LWindow()
{
	//Initialize non-existant window
	mWindow = NULL;
	mRenderer = NULL;

	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mShown = false;
	mWindowID = -1;
	
	mWidth = 0;
	mHeight = 0;
}

bool LWindow::init()
{
	//Create window and renderer
	if( !SDL_CreateWindowAndRenderer( "SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &mWindow, &mRenderer ) )
	{
		return false;
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = SCREEN_WIDTH;
	mHeight = SCREEN_HEIGHT;

	//Enable vsync
	SDL_SetRenderVSync( mRenderer, 1 );

	//Initialize renderer color
	SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

	//Grab window identifier
	mWindowID = SDL_GetWindowID( mWindow );

	//Flag as opened
	mShown = true;

	return true;
}

void LWindow::handleEvent( SDL_Event& e )
{
	//If an event was detected for this window
	if( e.type >= SDL_EVENT_WINDOW_FIRST && e.type <= SDL_EVENT_WINDOW_LAST && e.window.windowID == mWindowID )
	{
		//Caption update flag
		bool updateCaption = false;

		switch( e.type )
		{
			//Window appeared
			case SDL_EVENT_WINDOW_SHOWN:
			mShown = true;
			break;

			//Window disappeared
			case SDL_EVENT_WINDOW_HIDDEN:
			mShown = false;
			break;

			//Get new dimensions and repaint
			case SDL_EVENT_WINDOW_RESIZED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent( mRenderer );
			break;

			//Repaint on expose
			case SDL_EVENT_WINDOW_EXPOSED:
			SDL_RenderPresent( mRenderer );
			break;

			//Mouse enter
			case SDL_EVENT_WINDOW_MOUSE_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;
			
			//Mouse exit
			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			//Keyboard focus gained
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;
			
			//Keyboard focus lost
			case SDL_EVENT_WINDOW_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			//Window minimized
			case SDL_EVENT_WINDOW_MINIMIZED:
            mMinimized = true;
            break;

			//Window maximized
			case SDL_EVENT_WINDOW_MAXIMIZED:
			mMinimized = false;
            break;
			
			//Window restored
			case SDL_EVENT_WINDOW_RESTORED:
			mMinimized = false;
            break;

			//Hide on close
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				SDL_HideWindow( mWindow );
			break;
		}

		//Update window caption with new data
		if( updateCaption )
		{
			std::stringstream caption;
			caption << "SDL Tutorial - ID: " << mWindowID << " MouseFocus:" << ( ( mMouseFocus ) ? "On" : "Off" ) << " KeyboardFocus:" << ( ( mKeyboardFocus ) ? "On" : "Off" );
			SDL_SetWindowTitle( mWindow, caption.str().c_str() );
		}
	}
}

void LWindow::focus()
{
	//Restore window if needed
	if( !mShown )
	{
		SDL_ShowWindow( mWindow );
	}

	//Move window forward
	SDL_RaiseWindow( mWindow );
}

void LWindow::render()
{
	if( !mMinimized )
	{	
		//Clear screen
		SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( mRenderer );

		//Update screen
		SDL_RenderPresent( mRenderer );
	}
}

void LWindow::free()
{
	if( mWindow != NULL )
	{
		SDL_DestroyWindow( mWindow );
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth()
{
	return mWidth;
}

int LWindow::getHeight()
{
	return mHeight;
}

bool LWindow::hasMouseFocus()
{
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
	return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
	return mMinimized;
}

bool LWindow::isShown()
{
	return mShown;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( !SDL_Init( SDL_INIT_VIDEO ) )
	{
		SDL_Log( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		if( !gWindows[ 0 ].init() )
		{
			SDL_Log( "Window 0 could not be created!\n" );
			success = false;
		}
	}

	return success;
}

void close()
{
	//Destroy windows
	for( int i = 0; i < TOTAL_WINDOWS; ++i )
	{
		gWindows[ i ].free();
	}

	//Quit SDL subsystems
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		SDL_Log( "Failed to initialize!\n" );
	}
	else
	{
		//Initialize the rest of the windows
		for( int i = 1; i < TOTAL_WINDOWS; ++i )
		{
			gWindows[ i ].init();
		}

		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		//While application is running
		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
				//User requests quit
				if( e.type == SDL_EVENT_QUIT )
				{
					quit = true;
				}

				//Handle window events
				for( int i = 0; i < TOTAL_WINDOWS; ++i )
				{
					gWindows[ i ].handleEvent( e );
				}

				//Pull up window
				if( e.type == SDL_EVENT_KEY_DOWN )
				{
					switch( e.key.key )
					{
						case SDLK_1:
						gWindows[ 0 ].focus();
						break;

						case SDLK_2:
						gWindows[ 1 ].focus();
						break;
							
						case SDLK_3:
						gWindows[ 2 ].focus();
						break;
					}
				}
			}

			//Update all windows
			for( int i = 0; i < TOTAL_WINDOWS; ++i )
			{
				gWindows[ i ].render();
			}
				
			//Check all windows
			bool allWindowsClosed = true;
			for( int i = 0; i < TOTAL_WINDOWS; ++i )
			{
				if( gWindows[ i ].isShown() )
				{
					allWindowsClosed = false;
					break;
				}
			}

			//Application closed all windows
			if( allWindowsClosed )
			{
				quit = true;
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
