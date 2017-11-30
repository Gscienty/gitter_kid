const walk = require('walk'),
    fs = require('fs');

const walker = walk.walk('./src/routes', null);
let pages = [];

walker.on('file', (root, stats, next) => {
    if (stats.name.startsWith('_') || stats.name.endsWith('.js') === false) {
        next();
    }
    let folder = root.substring(1).replace('\\', '/').replace('/src', '');
    let basename = folder.replace('/routes', '');

    fs.readFile(`${root}/${stats.name}`, (err, data) => {
        let content = data.toString('utf-8');
        let uri_params = '';
        if (/\/\*.*?params\( *?\w+( *?, *?\w+)* *?\).*?\*\//.test(content)) {
            /\/\*.*?params\((.*)?\).*?\*\//
                .exec(content)[1]
                .replace(' ', '')
                .split(',')
                .forEach((param, index) => {
                    uri_params += (index === 0 ? '/' : '&') + ':' + param;
                });
        }

        let page_name = stats.name.split('.')[0];
        if (folder === '/routes' && page_name === 'index') { page_name = ''; }
        let page_meta = {
            path: `${basename}/${page_name}`,
            exact: page_name === '',
            component: `.${folder}/${page_name === '' ? 'index' : page_name}`
        };
        pages.push(JSON.stringify(page_meta));
        next();
    });
});
