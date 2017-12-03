import App from './penthesileia/penthesileia-bow';
import { HashRouter } from 'react-router-dom';
import 'antd/dist/antd.min.css'

let app = new App();

app.addPage({ path: '/:type&:keyword', exact: true, component: require('./routes/index').default })

app.run(document.getElementById('root'), HashRouter)