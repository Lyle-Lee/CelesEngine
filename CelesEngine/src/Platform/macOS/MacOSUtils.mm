#include "PCH.h"
#include "Celes/Utils/PlatformUtils.h"
#include "Celes/Core/Application.h"
#include <Cocoa/Cocoa.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace Celes {

	NSArray<NSString*>* ParseFileFilter(const char* filter)
	{
		if (!filter || filter[0] == '\0') return nil;

		NSMutableArray* fileTypes = [NSMutableArray array];

		// filter is a null-terminated string with pairs of (description, extensions)
		// e.g. "Text Files\0*.txt;*.md\0Image Files\0*.png;*.jpg\0\0"
		const char* ptr = filter;
		while (*ptr)
		{
			// the first part is the description, we can ignore it
			ptr += strlen(ptr) + 1;
			if (*ptr)
			{
				// get the extension pattern
				NSString* extensions = [NSString stringWithUTF8String:ptr];
				extensions = [extensions stringByReplacingOccurrencesOfString:@"*." withString:@""];
				extensions = [extensions stringByReplacingOccurrencesOfString:@"*" withString:@""];
				NSArray* extArray = [extensions componentsSeparatedByString:@";"];
				for (NSString* ext in extArray)
				{
					if (ext.length > 0)
					{
						[fileTypes addObject:ext];
					}
				}

				ptr += strlen(ptr) + 1;
			}
		}

		return fileTypes;
	}

	std::string FileDialogs::OpenFile(const char* filter)
	{
		@autoreleasepool
		{
        	// use NSOpenPanel for macOS file dialog
        	NSOpenPanel* openPanel = [NSOpenPanel openPanel];
        
        	// basic configuration
        	[openPanel setCanChooseFiles:YES];
        	[openPanel setCanChooseDirectories:NO];
        	[openPanel setAllowsMultipleSelection:NO];
        	[openPanel setMessage:@"Select a file"];
        
        	// set filter if provided
			NSArray<NSString*>* fileTypes = ParseFileFilter(filter);
			if (fileTypes && fileTypes.count > 0)
			{
				[openPanel setAllowedFileTypes:fileTypes];
			}
        
			// link to the current window
			NSWindow* nswindow = glfwGetCocoaWindow((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());

			// display modal dialog
			NSModalResponse response = [openPanel runModal];

			// process result
			if(response == NSModalResponseOK) {
				NSURL* url = [openPanel URLs].firstObject;
				return std::string([[url path] UTF8String]);
			}
        
        	return std::string();
    	}
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		@autoreleasepool
		{
			// use NSSavePanel for macOS file dialog
			NSSavePanel* savePanel = [NSSavePanel savePanel];

			// basic configuration
			[savePanel setCanCreateDirectories:YES];
			[savePanel setShowsTagField:NO];
			[savePanel setMessage:@"Save file"];
			[savePanel setNameFieldStringValue:@"Untitled"];
			// [savePanel setAllowedFileTypes:@[(NSString*)kUTTypeText]];

			// set filter if provided
			NSArray<NSString*>* fileTypes = ParseFileFilter(filter);
			if (fileTypes && fileTypes.count > 0)
			{
				[savePanel setAllowedFileTypes:fileTypes];
				[savePanel setAllowsOtherFileTypes:NO];
				[savePanel setExtensionHidden:NO];
				[savePanel setCanSelectHiddenExtension:YES];
			
				// set default file extension if the first type is not empty
				if (fileTypes.count > 0 && fileTypes[0].length > 0)
				{
					[savePanel setNameFieldStringValue:[NSString stringWithFormat:@"Untitled.%@", fileTypes[0]]];
				}
			}

			// link to the current window
			NSWindow* nswindow = glfwGetCocoaWindow((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());

			// display modal dialog
			if ([savePanel runModal] == NSModalResponseOK)
			{
				NSURL* url = [savePanel URL];

				// validate file extension against allowed types
				if (fileTypes.count > 0)
				{
					NSString* selectedExtension = [[url pathExtension] lowercaseString];
					BOOL validExtension = NO;
					for (NSString* allowedExt in fileTypes)
					{
						if ([selectedExtension isEqualToString:[allowedExt lowercaseString]])
						{
							validExtension = YES;
							break;
						}
					}

					if (!validExtension && selectedExtension.length == 0)
					{
						url = [url URLByAppendingPathExtension:fileTypes[0]];
					}
				}

				return std::string([[url path] UTF8String]);
			}

			return std::string();
		}
	}

	float Platform::GetTime()
	{
		return (float)glfwGetTime();
	}

}
