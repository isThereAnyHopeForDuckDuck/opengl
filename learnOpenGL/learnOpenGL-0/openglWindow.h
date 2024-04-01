#pragma once

#include <GLFW/glfw3.h>
#include <iostream>

class openglWindow
{
public:
	virtual			~openglWindow();
	bool			createWindow(int w, int h);	
	void			run();
	void			processInput(GLFWwindow* window);
	virtual void	doRender() {}
	virtual bool	onInit() { return true; }
	virtual void	onUninit() {}

	static void		framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

private:
	GLFWwindow* m_window;
};

