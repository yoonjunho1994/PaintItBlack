//This script searches for g_renderer related addresses.
//- DrawScene (Function address)
//- Renderer (Global address of the renderer object)
//@author Norman Ziebal
//@category pib
//@keybinding 
//@menupath 
//@toolbar 


import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.util.*;
import utilities.util.ArrayUtilities;
import ghidra.program.model.reloc.*;
import ghidra.program.model.data.*;
import ghidra.program.model.block.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.scalar.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.lang.*;
import ghidra.program.model.pcode.*;
import ghidra.program.model.address.*;

public class renderer extends GhidraScript {
	PiBLib pib;
	
	public void find_Renderer() throws MemoryAccessException
	{
		Address sig_addr = findBytes(null, "\\x8B\\x0D.{0,4}\\x8D\\x45.{1}\\x50\\x57\\x53");
		pib.log("find_Renderer: addr: 0x" + sig_addr);
		if (sig_addr == null) {
			printerr("find_Renderer: signatur not found!");
			return;
		}
		
		byte instr[] = new byte[6];
		int ret = currentProgram.getMemory().getBytes(sig_addr, instr);
		if (ret != instr.length) {
			printerr("find_Renderer: failed to get all bytes.");
			return;
		}
		instr = ArrayUtilities.reverse(instr);
		
		print("Renderer: 0x");
		int offset_mov_instr = 2;
		for (int i = 0; i < instr.length - offset_mov_instr; i++) {
			print(Integer.toHexString(instr[i] & 0xff));
		}
		print("\n");
	}

	public void find_DrawScene()
	{
		Set<Function> func_set = pib.string_to_reflist("3dlib\\Renderer.cpp");
		
		if (func_set.size() > 1) {
			printerr("find_DrawScene: failed because found more then one function!");
			return;
		}
		
		Function DrawScene_addr = null;
		for(Iterator<Function> it = func_set.iterator(); it.hasNext();)
			DrawScene_addr = it.next();
			
		print("DrawScene: 0x" + DrawScene_addr.getEntryPoint() + "\n");
	}
	
    public void run() throws Exception
    {
    	pib = new PiBLib(false, "none", this);
    	find_Renderer();
    	find_DrawScene();
    }
}
