//Core of the helper classes
//@author Norman Ziebal
//@category pib.lib
//@keybinding 
//@menupath 
//@toolbar 

import java.util.HashSet;
import java.util.Set;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.ReferenceIterator;

public class PiBLib {
	Logger logger;
	GhidraScript caller;
	
	public PiBLib(boolean debug, String dfunc, GhidraScript caller) {
		logger = new Logger(debug, dfunc, caller);
		this.caller = caller;
	}

	public <T> void log(T msg) {
		logger.print(msg);
	}
	
	public Set<Function> string_to_reflist(String str) {
		Address str_addr = caller.find(str);
		logger.print("find_DrawScene: str_addr: 0x" + str_addr);
		ReferenceIterator iter = caller.getCurrentProgram().getReferenceManager().getReferencesTo(str_addr);
		
		Set<Function> func_set = new HashSet<>();
		FunctionManager fm = caller.getCurrentProgram().getFunctionManager();
		for(ReferenceIterator it = iter; it.hasNext();) {
			func_set.add(fm.getFunctionContaining(it.next().getFromAddress()));
		}
		return func_set;
	}

}
