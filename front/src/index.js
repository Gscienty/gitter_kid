import App from './penthesileia/penthesileia-bow';
import { HashRouter } from 'react-router-dom';
import 'antd/dist/antd.min.css'

let app = new App();

app.addModel(require('./models/repository').default);

app.addPage({ path: '/', exact: true, component: require('./routes/index').default })
app.addPage({ path: '/query/:keyword?', exact: true, component: require('./routes/query').default })

app.run(document.getElementById('root'), HashRouter)