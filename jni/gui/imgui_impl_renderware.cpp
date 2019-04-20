#include "main.h"
#include "gui.h"
#include "game/game.h"

#include "game/RW/RenderWare.h"

extern CGUI *pGUI;

RwRaster* g_FontRaster = nullptr;

RwIm2DVertex* g_pVB = nullptr; // vertex buffer
unsigned int g_VertexBufferSize = 5000;
ImVec4 g_ScissorRect;

void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data)
{
	const RwReal nearScreenZ = 		*(RwReal*)(g_libGTASA+0x9DAA60);	// CSprite2d::NearScreenZ 009DAA60
	const RwReal recipNearClip = 	*(RwReal*)(g_libGTASA+0x9DAA64);	// CSprite2d::RecipNearClip 009DAA64

	if(!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
	{
		if(g_pVB) { delete g_pVB; g_pVB = nullptr; }
		g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
		g_pVB = new RwIm2DVertex[g_VertexBufferSize];
		if(!g_pVB)
		{
			Log("GUI | Error: couldn't allocate vertex buffer (size: %d)", g_VertexBufferSize);
			return;
		}
		Log("GUI | Vertex buffer reallocated. Size: %d", g_VertexBufferSize);
	}

	RwIm2DVertex* vtx_dst = g_pVB;
	int vtx_offset = 0;

	for(int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_src = cmd_list->IdxBuffer.Data;

        for(int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
        	RwIm2DVertexSetScreenX(vtx_dst, vtx_src->pos.x);
        	RwIm2DVertexSetScreenY(vtx_dst, vtx_src->pos.y);
        	RwIm2DVertexSetScreenZ(vtx_dst, nearScreenZ);
        	RwIm2DVertexSetRecipCameraZ(vtx_dst, recipNearClip);
        	vtx_dst->emissiveColor = vtx_src->col;
        	RwIm2DVertexSetU(vtx_dst, vtx_src->uv.x, recipCameraZ);
        	RwIm2DVertexSetV(vtx_dst, vtx_src->uv.y, recipCameraZ);

        	vtx_dst++;
        	vtx_src++;
        }

        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
        for(int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
        	const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

        	if(pcmd->UserCallback)
        	{
        		pcmd->UserCallback(cmd_list, pcmd);
        	}
        	else
        	{
        		g_ScissorRect.x = pcmd->ClipRect.x;
        		g_ScissorRect.y = pcmd->ClipRect.w;
        		g_ScissorRect.z = pcmd->ClipRect.z;
        		g_ScissorRect.w = pcmd->ClipRect.y;
        		SetScissorRect((void*)&g_ScissorRect);

        		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)0);
  				RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)0);
  				RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
  				RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
  				RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
  				RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)0);
  				RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
  				RwRenderStateSet(rwRENDERSTATEBORDERCOLOR, (void*)0);
  				RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONGREATER);
  				RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)2);
  				RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
  				RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
  				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pcmd->TextureId);
  				RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, 
  					&g_pVB[vtx_offset], (RwInt32)cmd_list->VtxBuffer.Size,
  					(RwImVertexIndex*)idx_buffer, pcmd->ElemCount);
  				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);

  				g_ScissorRect.x = 0;
        		g_ScissorRect.y = 0;
        		g_ScissorRect.z = 0;
        		g_ScissorRect.w = 0;
        		SetScissorRect((void*)&g_ScissorRect);
        	}

        	idx_buffer += pcmd->ElemCount;
        }
        vtx_offset += cmd_list->VtxBuffer.Size;
	}

	return;
}

bool ImGui_ImplRenderWare_Init()
{
	ImGuiIO &io = ImGui::GetIO();

	io.DisplaySize = ImVec2((float)RsGlobal->maximumWidth, RsGlobal->maximumHeight);
	Log("GUI | Display size: %f, %f", io.DisplaySize.x, io.DisplaySize.y);

	return true;
}

bool ImGui_ImplRenderWare_CreateDeviceObjects()
{
	Log("GUI | CreateDeviceObjects.");
	if(g_FontRaster) Log("GUI | Warning: Font raster != 0");

	// Build texture atlas
	ImGuiIO &io = ImGui::GetIO();
	unsigned char* pxs;
	int width, height, bytes_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pxs, &width, &height, &bytes_per_pixel);
	Log("GUI | Font atlas width: %d, height: %d, depth: %d", width, height, bytes_per_pixel*8);

	RwImage *font_img = RwImageCreate(width, height, bytes_per_pixel*8);
	RwImageAllocatePixels(font_img);

	RwUInt8 *pixels = font_img->cpPixels;
	for(int y = 0; y < font_img->height; y++)
	{
		memcpy((unsigned char*)pixels, pxs + font_img->stride * y, font_img->stride);
		pixels += font_img->stride;
	}

	RwInt32 w, h, d, flags;
	RwImageFindRasterFormat(font_img, rwRASTERTYPETEXTURE, &w, &h, &d, &flags);
	g_FontRaster = RwRasterCreate(w, h, d, flags);
	g_FontRaster = RwRasterSetFromImage(g_FontRaster, font_img);
	RwImageDestroy(font_img);

	io.Fonts->TexID = (ImTextureID*)g_FontRaster;
	return true;
}

void ImGui_ImplRenderWare_ShutDown()
{
	Log("ImGui ShutDown.");

	ImGuiIO &io = ImGui::GetIO();

	// destroy raster
	RwRasterDestroy(g_FontRaster);
	g_FontRaster = nullptr;
	io.Fonts->TexID = nullptr;

	// destroy vertex buffer
	if(g_pVB) { delete g_pVB; g_pVB = nullptr; }
	return;
}

void ImGui_ImplRenderWare_NewFrame()
{
	if(!g_FontRaster)
		ImGui_ImplRenderWare_CreateDeviceObjects();

	ImGuiIO &io = ImGui::GetIO();

}