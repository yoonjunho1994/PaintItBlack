import ghidra.app.script.GhidraScript;

//A helper class for logging debug messages.
//@author Norman Ziebal
//@category pib.lib
//@keybinding 
//@menupath 
//@toolbar 
public class Logger {
	private boolean debug = false; // If this is set all debug prints are displayed.
	private String dfunc = "none"; // Print only debug from this function. 
	GhidraScript caller;
	
	public Logger(boolean debug, String dfunc,  GhidraScript caller) {
		this.debug = debug;
		this.dfunc = dfunc;
		this.caller = caller;
	}
	
	public <T> void print(T msg)
	{
		if (debug || Thread.currentThread().getStackTrace()[3].getMethodName().equals(dfunc))
			caller.print(msg.toString() + "\n");
	}

	public boolean isDebug() {
		return debug;
	}

	public void setDebug(boolean debug) {
		this.debug = debug;
	}

	public String getDfunc() {
		return dfunc;
	}

	public void setDfunc(String dfunc) {
		this.dfunc = dfunc;
	}
}
