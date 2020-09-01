setlocal enabledelayedexpansion
@echo off
for %%v in (*.vert) do (
	echo %%v
	%VULKAN_SDK%\Bin\glslc.exe %%v -o %%v.spv
)
for %%f in (*.frag) do (
	echo %%f
	%VULKAN_SDK%\Bin\glslc.exe %%f -o %%f.spv
)