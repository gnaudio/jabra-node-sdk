import { app, BrowserWindow, ipcMain } from "electron";
import { ConfigParamsCloud } from '@gnaudio/jabra-node-sdk';
import * as process from 'process';
import { JabraApiServerFactory, JabraApiServer } from '@gnaudio/jabra-electron-renderer-helper';

import * as os from "os";
import * as path from "path";

// Get our version information directly from packages (special need for our demo/test - properly not 
// something most applications would need):
const testPackage = require ('../../package.json');
const nodeSdkPackage = require ( '@gnaudio/jabra-node-sdk/package.json');
const electronHelperPackage = require ('@gnaudio/jabra-electron-renderer-helper/package.json');
const osType = `${os.platform()} (${os.arch()})`;

// Globals used by our demo:
let mainWindow: BrowserWindow | null = null;
let jabraServerFactory : JabraApiServerFactory | null = null;
let jabraServer: JabraApiServer | null = null;

/**
 * Create electon window returning a promise that resolves when
 * the window is fully loaded and thus ready to receive events.
 */
function createAndLoadWindow(): Promise<BrowserWindow> {
  // Create the browser window.
  let window = new BrowserWindow({
    height: 600,
    width: 800,
    webPreferences: {
      // Disabled Node integration
      nodeIntegration: false,
      // In a sandbox
      sandbox: true,
      // Allow Ipc to/from sandbox
      contextIsolation: false,
      // No insecure code.
      webSecurity: true,
      // Preload script
      preload: path.join(__dirname, 'preload.js')
    }
  });

  // Our demo app need to show version info for componets. Most are
  // are passed as query args to our renderer window:
  const args = `testAppVersion=${testPackage.version}
                &nodeSdkVersion=${nodeSdkPackage.version}
                &electronHelperVersion=${electronHelperPackage.version}
                &electronVersion=${process.versions.electron}
                &nodeVersion=${process.versions.node}
                &osType=${osType}`.replace(/[\n\r\t\s]/g, '');

  const htmlUrl = `file://${__dirname}/../renderer/index.html?${args}`

  // load the index.html of the app returning a promise that resolves when loaded.
  // Nb. the promise part is new for electron 5 - alternatively, we could wait for
  // 'did-finish-load' and convert that into a promise.

  const loadPromise = window.loadURL(htmlUrl);

  // Open the DevTools.
  // window.webContents.openDevTools();

  // Emitted when the window is closed.
  window.on("closed", () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;
    shutdown();
  });
 
  window.on('unresponsive', () => {
    console.log('window crashed');
  });


  window.webContents.on('did-fail-load', () => {
    console.log('window failed load');
  });

  return loadPromise.then(() => {      
    mainWindow = window;
    return window;
  });
}

function shutdown() {
  if (jabraServer) {
     jabraServer.shutdown().catch((err) => {
      console.log("Error during server shutdown " + err);
     });

    jabraServer = null;
  }
  if (app) {
    app.quit();
  }
}

function setup() {
  // First setup the jabra server factory singleton when electron is initialized but BEFORE creating GUI.
  if (!jabraServerFactory) {
    jabraServerFactory = new JabraApiServerFactory(ipcMain);
  }

  createAndLoadWindow().then((fullyLoadedWindow) => {   
    let config: ConfigParamsCloud = {
      // Set any needed configuration parameters here.
    };

    let allowNonJabraDevices = false;

    // As window is now fully loaded we can create our api server for the client.
    jabraServerFactory!.create('A7tSsfD42VenLagL2mM6i2f0VafP/842cbuPCnC+uE8=', config, allowNonJabraDevices, fullyLoadedWindow).then( (result) => {
      jabraServer = result;
      console.log("JabraApiServer sucessfully created");

      // If the main process need to use the JabraApi, here is the 
      // reference to it:
      const jabraApi = result.getJabraApi();

      return jabraServer;
    }).catch( (err) => {
      console.log("Error during jabra application/server setup " + err);
    });
  });
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on("ready", async () => {
  // Gui setup
  setup();
});

// Quit when all windows are closed.
app.on("window-all-closed", () => {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== "darwin") {
    shutdown();
  }
});

app.on("activate", () => {
  // On OS X it"s common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    setup();
  }
});

process.on('uncaughtException', (errorObject) => {
  let error : string = "";
  if (errorObject instanceof Error) {
    error = 'UnhandledRejection ' + errorObject.name;
    return;
  } else {
    error = JSON.stringify(errorObject);
  }

  console.error(error);
});

process.on('unhandledRejection', (rejection) => {
  let error : string = "";
  if (rejection instanceof Error) {
    error = 'UnhandledRejection ' + rejection.name;
    return;
  } else {
    error = JSON.stringify(rejection);
  }

  console.error(error);
});
