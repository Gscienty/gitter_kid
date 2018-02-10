export default function pathTransferItems (repositoryName, path) {
    let treePath = path;
    if (treePath.endsWith('/')) {
        treePath = treePath.substring (0, treePath.length - 1);
    }
    let pathItems = treePath.split('/');
    pathItems[0] = { name: '', path: '' };
    for (let i = 1; i < pathItems.length; i++) {
        pathItems[i] = { name: pathItems[i], path: pathItems[i - 1].path + '/' + pathItems[i] };
    }

    pathItems[0].name = repositoryName;
    pathItems[0].path = '/';

    return pathItems;
}