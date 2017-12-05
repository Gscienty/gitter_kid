import App from './penthesileia/penthesileia-bow';
import { HashRouter } from 'react-router-dom';
import 'antd/dist/antd.min.css';
import './style/code-syntax-highlight.css';

let app = new App();

app.addModel(require('./models/repository').default);

app.addPage({ path: '/', exact: true, component: require('./routes/index').default })
app.addPage({ path: '/query/:keyword?', exact: true, component: require('./routes/query').default })
app.addPage({ path: '/repository/:repository/:tag', exact: true, component: require('./routes/repository/index').default })

app.run(document.getElementById('root'), HashRouter)