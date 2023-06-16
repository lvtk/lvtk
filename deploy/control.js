function Controller()
{
    installer.componentAdded.connect (this, Controller.prototype.onComponentAdded);
}

Controller.prototype.onComponentAdded = function (comp) {
    if (! comp.name.includes ("org.lvtk.")) {
        console.log ("Added: " + comp.name);
    }
}
