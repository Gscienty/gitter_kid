import React from 'react';
import BasePage from '../components/BasePage'
import { connect } from 'react-redux';
import { Card } from 'antd';
class Page extends BasePage {
    page() {
        return <div>
            <Card style={{
                textAlign: 'center',
                fontFamily: 'Courier New'
            }}>
                <h1> Hello World. </h1>
                <h3>This is <b>Gitter Kid</b> project. Help you manage Git private repositories more easily.</h3>
                
                <div style={{ marginTop: 21 }}>
                    <p>The world has kissed my soul with its pain,</p>
                    <p>asking for its return in songs.</p>
                </div>
            </Card>
        </div>;
    }
}

export default connect()(Page);