let project = new Project('LudumDare43');

project.addFile('Sources/**');
project.setDebugDir('Deployment');

resolve(project);
