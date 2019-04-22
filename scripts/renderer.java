//TODO write a description for this script
//@author 
//@category _NEW_
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
	boolean debug = true;
	
	public void find_Renderer() throws MemoryAccessException
	{
		Address sig_addr = findBytes(null, "\\x8B\\x0D.{0,4}\\x8D\\x45.{1}\\x50\\x57\\x53");
		debug_print("find_Renderer: addr: 0x" + sig_addr);
		if (sig_addr == null) {
			System.out.println("find_Renderer: signatur not found!");
			return;
		}
		
		byte instr[] = new byte[6];
		int ret = currentProgram.getMemory().getBytes(sig_addr, instr);
		if (ret != instr.length) {
			System.out.println("find_Renderer: failed to get all bytes.");
			return;
		}
		instr = ArrayUtilities.reverse(instr);
		
		System.out.print("Renderer: 0x");
		for (int i = 0; i < instr.length - 2; i++) {
			System.out.print(Integer.toHexString(instr[i] & 0xff));
		}
		System.out.println();
	}

	public void find_DrawScene()
	{
		Address str_addr = find("3dlib\\Renderer.cpp");
		debug_print("find_DrawScene: str_addr: 0x" + str_addr);
		ReferenceIterator iter = currentProgram.getReferenceManager().getReferencesTo(str_addr);
		
		Set<Function> func_set = new HashSet<>();
		FunctionManager fm = currentProgram.getFunctionManager();
		for(ReferenceIterator it = iter; it.hasNext();) {
			func_set.add(fm.getFunctionContaining(it.next().getFromAddress()));
		}
		
		if (func_set.size() > 1) {
			System.out.println("find_DrawScene: failed because found more then one function!");
			return;
		}
		
		Function DrawScene_addr = null;
		for(Iterator<Function> it = func_set.iterator(); it.hasNext();)
			DrawScene_addr = it.next();
			
		System.out.println("DrawScene: 0x" + DrawScene_addr.getEntryPoint());
	}
	
    public void run() throws Exception
    {
    	find_Renderer();
    	find_DrawScene();
    }
    
	public <T> void debug_print(T msg)
	{
		if (debug)
			System.out.println(msg);
	}
}
