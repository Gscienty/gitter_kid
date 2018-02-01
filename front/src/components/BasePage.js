import React, { Component } from 'react';
import { Layout, Menu } from 'antd';

export default class BasePage extends Component {
    page() { return <div></div> }

    handleMenuOnClick(e) {
        switch (e.key) {
            case 'repositories':
                this.props.history.push('/repositories/default');
                break;
            default: break;
        }
    }

    render() {
        return <Layout>
            <Layout.Header>
                <a
                    style={{ color: '#ffffff', fontSize: '27px', float: 'left', marginRight: '28px' }}
                    onClick={ e => this.props.history.push('/') }>
                    Gitter Kid
                </a>

                <div style={{ float: 'left', marginRight: '28px' }}>
                    <Menu
                        theme="dark"
                        mode="horizontal"
                        style={{ lineHeight: '64px' }}
                        onClick={ this.handleMenuOnClick.bind(this) }
                        selectedKeys={[ 
                            this.props.entryUnit
                        ]}>
                        <Menu.Item key="repositories" onClick={ () => console.log ('HERE') }>Repositories</Menu.Item>
                    </Menu>
                </div>
            </Layout.Header>
            <Layout.Content style={{ padding: '20px 50px', background: '#ffffff' }}>
                <div>
                    { this.page() }
                </div>
            </Layout.Content>
            <Layout.Footer style={{ textAlign: 'center', background: '#ffffff' }}>
            </Layout.Footer>
        </Layout>
    }
}