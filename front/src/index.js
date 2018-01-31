import App from './penthesileia/penthesileia-bow';
import { HashRouter } from 'react-router-dom';
import 'antd/dist/antd.min.css';
import './style/code-syntax-highlight.css';

let app = new App();

app.addModel(require('./models/repositories').default);
app.addModel(require('./models/tree').default);
app.addModel(require('./models/blob').default);
app.addModel(require('./models/branches').default);

app.addPage({ path: '/', exact: true, component: require('./routes/index').default });
app.addPage({ path: '/repositories/:repositoriesName', exact: true, component: require('./routes/repositories').default });
app.addPage({ path: '/repository/:repositoriesName/:repositoryName/:branchName/tree/*', exact: true, component: require('./routes/repository/tree').default });
app.addPage({ path: '/repository/:repositoriesName/:repositoryName/:branchName/blob/*', exact: true, component: require('./routes/repository/blob').default });

app.run(document.getElementById('root'), HashRouter)