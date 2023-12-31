#include <pch.h>
#include <base.h>

//Functions

bool Base::Hooks::Init()
{
	Data::pSwapBuffers = mem::in::get_symbol(mem::in::get_module("opengl32.dll"), "wglSwapBuffers");
	for(int i = 0; i < 2; i++)
		Data::pSwapBuffers = (mem::voidptr_t)((uintptr_t)Data::pSwapBuffers + *(uintptr_t*)(&((mem::byte_t*)Data::pSwapBuffers)[1]) + 5);

#	if defined(MEM_86)
	Data::oSwapBuffers = (SwapBuffers_t)mem::in::detour_trampoline(Data::pSwapBuffers, Hooks::SwapBuffers, Data::szSwapBuffers, mem::MEM_DT_M1);
#	elif defined(MEM_64)
	Data::oSwapBuffers = (SwapBuffers_t)mem::in::detour_trampoline(Data::pSwapBuffers, Hooks::SwapBuffers, Data::szSwapBuffers);
#	endif
	

	Data::m_hw->cl_funcs->pCL_CreateMove    = Hooks::CL_CreateMove;
	Data::m_hw->cl_funcs->pCL_IsThirdPerson = Hooks::CL_IsThirdPerson;
	Data::m_hw->cl_funcs->pClientMove       = Hooks::HUD_ClientMove;

	Base::Hacks::InitHacks();

	return true;
}

bool Base::Hooks::Shutdown()
{
	if (Data::InitImGui)
	{
		ImGui_ImplOpenGL2_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	Base::Hacks::ShutdownHacks();
	//Sleep(100);

	mem::in::detour_restore(Data::pSwapBuffers, (mem::byte_t*)Data::oSwapBuffers, Data::szSwapBuffers);
	SetWindowLongPtr(Data::hWindow, WNDPROC_INDEX, (LONG_PTR)Data::oWndProc);
	memcpy(Data::m_hw->cl_funcs, &Data::m_hw->o_cl_funcs, sizeof(Data::m_hw->o_cl_funcs));

	return true;
}