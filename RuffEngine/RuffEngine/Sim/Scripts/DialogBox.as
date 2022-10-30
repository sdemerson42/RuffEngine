ScriptComponent@ gDialogBox;

void DialogBox_Main(ScriptComponent@ api)
{
	@gDialogBox = api;
	api.SetComponentActive("render", false);
}
