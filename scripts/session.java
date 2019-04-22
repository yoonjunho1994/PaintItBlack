//TODO write a description for this script
//@author 
//@category pib
//@keybinding 
//@menupath 
//@toolbar 


import ghidra.app.script.GhidraScript;
import ghidra.program.model.util.*;
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

public class session extends GhidraScript {
	Logger logger;
	
	public void find_GetTalkType()
	{
		
	}
	
    public void run() throws Exception 
    {
    	//logger = new Logger(false, "find_GetTalkType");
    	find_GetTalkType();
    }

}
