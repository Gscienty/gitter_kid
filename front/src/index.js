import App from './penthesileia/penthesileia-bow';
import { HashRouter } from 'react-router-dom';

let app = new App();

app.addModel(require('./models/category').default);

app.addPage({ path: '/', exact: true, component: require('./router/index').default })

app.run(document.getElementById('root'), HashRouter)